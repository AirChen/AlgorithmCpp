#pragma once

#include <math.h>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <vector>
#include "spatial-base.h"
#include "type_factory.h"
#include "get_data.h"
#include "mif_writer.h"

namespace mapimage {
    int area2(wsl::Point& p, wsl::Point& q, wsl::Point& s);
    bool is_left(wsl::Point& p, wsl::Point& q, wsl::Point& s);
    bool is_in_triangle(wsl::Polygon& tri, wsl::Point& pt);
    bool is_intersect(wsl::Point& l1p1, wsl::Point& l1p2, wsl::Point& l2p1, wsl::Point& l2p2);
    bool get_insect_point(wsl::Point& l1p1, wsl::Point& l1p2, wsl::Point& l2p1, wsl::Point& l2p2, wsl::Point& out);

    // 扩大多边形
    void expand_polygon(wsl::Polygon& ply, double safeline, wsl::Polygon &out, bool debug = false);

    void create_filter_kinds(std::set<std::string>& out_set);
    inline bool mbr_contain_pt(const wsl::MBR& mbr, const wsl::Point& pt) {
        if (mbr.ll.x() > pt.x() || mbr.ll.y() > pt.y() || mbr.ru.x() < pt.x() || mbr.ru.y() < pt.y()) {
            return false;
        }
        return true;
    };

    // 点到直线的距离
    double pt_distance_line(wsl::Point& pt, wsl::Point& w, wsl::Point& v);

    // 两点之间的距离
    double distance_line(wsl::Point& p1, wsl::Point& p2);
    double len_point_in_line(double len, wsl::Point& p1, wsl::Point& p2, std::vector<wsl::Point>& out);

    int gbk_to_utf8(std::string& str);

    int generate_date_seconds();
    bool is_outdate(int time_day, int begin_time, int end_time);

    void collision_general(std::vector<VirtualPoi*>& vpois, double dis);
    void check_dis(std::vector<VirtualPoi*>& vpois, double dis, int pre_level);
    void static_vpois(std::vector<VirtualPoi*>& vpois);
    void collision_vpoi_with_level_normal(std::vector<VirtualPoi*>& vpois, double dis, int base_l, int increase_l, int rank, ShowType st);
    
    // vonroi diagram applys
    void filter_near_tree(std::vector<VirtualPoi*>& vpois, std::vector<TreeData*>& trees, double len);
    void collision_vpoi_with_level(std::vector<VirtualPoi*>& vpois, double dis, int base_l, int increase_l, int rank, ShowType st);
    void filter_near_vpoi(std::vector<VirtualPoi*>& vpois, double dis);
}
