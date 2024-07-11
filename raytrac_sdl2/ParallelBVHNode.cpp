#include "ParallelBVHNode.h"


std::atomic<int> ParallelBVHNode::active_threads(0);

ParallelBVHNode::ParallelBVHNode(const std::vector<std::shared_ptr<Hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1) {

    std::vector<std::shared_ptr<Hittable>> objects(src_objects.begin() + start, src_objects.begin() + end);
    size_t object_span = objects.size();

    auto comparator = (rand() % 3 == 0) ? box_x_compare
        : (rand() % 2 == 0) ? box_y_compare
        : box_z_compare;

    if (object_span <= 2) {
        if (object_span == 1) {
            left = right = objects[0];
        }
        else {
            if (comparator(objects[0], objects[1])) {
                left = objects[0];
                right = objects[1];
            }
            else {
                left = objects[1];
                right = objects[0];
            }
        }
    }
    else {
        std::sort(objects.begin(), objects.end(), comparator);

        auto mid = object_span / 2;

        if (object_span >= MIN_OBJECTS_PER_THREAD && active_threads < std::thread::hardware_concurrency()) {
            std::future<std::shared_ptr<ParallelBVHNode>> future_left = std::async(
                std::launch::async,
                [&]() -> std::shared_ptr<ParallelBVHNode> {
                    active_threads++;
                    auto node = std::make_shared<ParallelBVHNode>(objects, 0, mid, time0, time1);
                    active_threads--;
                    return node;
                }
            );

            right = std::make_shared<ParallelBVHNode>(objects, mid, object_span, time0, time1);
            left = future_left.get();
        }
        else {
            left = std::make_shared<ParallelBVHNode>(objects, 0, mid, time0, time1);
            right = std::make_shared<ParallelBVHNode>(objects, mid, object_span, time0, time1);
        }
    }

    AABB box_left, box_right;
    if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in BVHNode constructor.\n";

    box = surrounding_box(box_left, box_right);
}

bool ParallelBVHNode::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}
bool ParallelBVHNode::bounding_box(double time0, double time1, AABB& output_box) const {
    output_box = box;
    return true;
}
bool ParallelBVHNode::box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) {
    AABB box_a;
    AABB box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in BVHNode constructor.\n";

    switch (axis) {
    case 0: return box_a.min.x < box_b.min.x;
    case 1: return box_a.min.y < box_b.min.y;
    case 2: return box_a.min.z < box_b.min.z;
    default:
        std::cerr << "Invalid axis in box_compare\n";
        return false;
    }
}

bool ParallelBVHNode::box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
}

bool ParallelBVHNode::box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
}

bool ParallelBVHNode::box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
}