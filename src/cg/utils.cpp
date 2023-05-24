#include "utils.h"
#include <cmath>
#include "uconv.h"
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/foreach.hpp>
#include <boost/polygon/voronoi.hpp>
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using boost::polygon::voronoi_edge;
using boost::polygon::voronoi_cell;
using boost::polygon::x;
using boost::polygon::y;

namespace boost {
    namespace polygon {
        template <>
        struct geometry_concept<wsl::Point> {
            typedef point_concept type;
        };

        template <>
        struct point_traits<wsl::Point> {
            typedef int coordinate_type;

            static inline coordinate_type get(
                const wsl::Point& point, orientation_2d orient) {
                return (orient == HORIZONTAL) ? point.x() : point.y();
            }
        };
    }
}

namespace bg = boost::geometry;

namespace mapimage {
    typedef boost::geometry::model::d2::point_xy<double> point;
    typedef boost::geometry::model::segment<point> segment;
    typedef boost::geometry::model::polygon<point> polygon;

    int area2(wsl::Point& p, wsl::Point& q, wsl::Point& s) {
        return p.x() * q.y() - p.y() * q.x() + q.x() * s.y() - q.y() * s.x() + s.x() * p.y() - s.y() * p.x();
    }

    bool is_left(wsl::Point& p, wsl::Point& q, wsl::Point& s) {
        return area2(p, q, s) > 0;
    }

    bool is_in_triangle(wsl::Polygon& tri, wsl::Point& pt) {
        if (tri.size() != 3) {
            return false;
        }

        if (tri.is_negative()) {
            for (size_t i = 2; i > -1; i--) {
                wsl::Point p = tri[i];
                wsl::Point q = tri[(i - 1 + 3) % 3];
                if (!is_left(p, q, pt)) {
                    return false;
                }
            }
        } else {
            for (size_t i = 0; i < 3; i++) {
                wsl::Point p = tri[i];
                wsl::Point q = tri[(i + 1) % 3];
                if (!is_left(p, q, pt)) {
                    return false;
                }
            }
        }

        return true;
    }

    bool is_intersect(wsl::Point& l1p1, wsl::Point& l1p2, wsl::Point& l2p1, wsl::Point& l2p2) {
        bool v1 = is_left(l1p1, l1p2, l2p1) & is_left(l1p1, l1p2, l2p2);
        bool v2 = is_left(l2p1, l2p2, l1p1) & is_left(l2p1, l2p2, l1p2);

        return !v1 && !v2;
    }

    double distance_line(wsl::Point& p1, wsl::Point& p2) {
        return sqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
    }

    double len_point_in_line(double len, wsl::Point& p1, wsl::Point& p2, std::vector<wsl::Point>& out) {
        double line_len = distance_line(p1, p2);
        if (len > line_len) {
            return (len - line_len);
        }

        double cur_len = 0.0;
        while (cur_len + len < line_len) {
            cur_len += len;

            double radio = cur_len / line_len;
            double x = (p2.x() - p1.x()) * radio + p1.x();
            double y = (p2.y() - p1.y()) * radio + p1.y();

            wsl::Point pt(x, y);
            out.emplace_back(pt);
        
        }
        return (line_len - cur_len);
    }

    void create_filter_kinds(std::set<std::string>& out_set) {
        out_set.insert("0121");
        out_set.insert("0122");
        out_set.insert("0123");
        out_set.insert("0125");
        out_set.insert("bd0126");
        out_set.insert("bd0127");
        out_set.insert("bd0128");
        out_set.insert("bd0182");
        out_set.insert("bd0183");
        out_set.insert("bd0184");
        out_set.insert("bd0185");
        out_set.insert("bd0186");
        out_set.insert("bd0189");
        out_set.insert("bd0190");
    }

    void expand_polygon(wsl::Polygon& ply, double safeline, wsl::Polygon &out, bool debug) {
        if (!ply.is_negative()) {
            safeline *= -1;
        }

        std::vector<wsl::Point> dpList, ndpList;
        size_t size = ply.size();

        wsl::Point l_pt = ply[size-1];
        wsl::Point f_pt = ply[0];
        if (fabs(l_pt.x() - f_pt.x()) < 0.000001 && fabs(l_pt.y() - f_pt.y()) < 0.000001) {
            size -= 1;
        }

        for(size_t i = 0; i < size; i++) {
            wsl::Point ppt = ply[(i + 1) % size] - ply[i];
            // if (ppt.x() < 1 || ppt.y() < 1) continue;

            dpList.emplace_back(ppt);
            double unitLen = 1.0 / sqrt(dpList[i].x() * dpList[i].x() + dpList[i].y() * dpList[i].y());
            wsl::Point pt(dpList[i].x() * unitLen, dpList[i].y() * unitLen);
            ndpList.emplace_back(pt);

            if (debug) {
                std::cout << "ndp list: <" << std::to_string(pt.x()) << ", " << std::to_string(pt.y()) << ">" << std::endl;
            }
        }
    
        wsl::Point first_pt(0.0, 0.0);
        for(int i = 0; i < size; i++) {
            int startIndex = (i - 1) % size;
            int endIndex = i;

            double sinTheta = ndpList[startIndex].x() * ndpList[endIndex].y() - ndpList[startIndex].y() * ndpList[endIndex].x();
            wsl::Point orientVector = ndpList[endIndex] - ndpList[startIndex];
            double x = ply[i].x() + safeline / sinTheta * orientVector.x();
            double y = ply[i].y() + safeline / sinTheta * orientVector.y();

            if (std::isnan(x)|| std::isnan(y) 
                || sqrt((x - ply[i].x()) * (x - ply[i].x()) + (y - ply[i].y()) * (y - ply[i].y())) > 50) {
                double tmp_x = (orientVector.x() > 0) ? 1 : -1;
                double tmp_y = (orientVector.y() > 0) ? 1 : -1;
                x = ply[i].x() + safeline / tmp_x;
                y = ply[i].y() + safeline / tmp_y;                
            }

            if (debug) {
                std::cout << "debug: <" << std::to_string(ply[i].x()) << ", " << std::to_string(ply[i].y()) << ">" 
                    << " vect <" << std::to_string(orientVector.x()) << ", " << std::to_string(orientVector.y()) << ">"
                    << " ndp start <"<< std::to_string(ndpList[startIndex].x()) << ", " << std::to_string(ndpList[startIndex].y()) << ">"
                    << " ndp end <"<< std::to_string(ndpList[endIndex].x()) << ", " << std::to_string(ndpList[endIndex].y()) << "> sin "
                    << std::to_string(sinTheta) << std::endl;
            }

            wsl::Point temp_out(x, y);
            out.push_back(temp_out);

            if (fabs(first_pt.x()) < 0.000001) {
                first_pt = temp_out;
            }

            if (debug) {
                std::cout << "before: <" << std::to_string(ply[i].x()) << ", " << std::to_string(ply[i].y()) << ">" << std::endl;
                std::cout << "after: <" << std::to_string(x) << ", " << std::to_string(y) << ">" << std::endl;
            }
        }
        out.push_back(first_pt);

        // print_times++;
    }

    double pt_distance_line(wsl::Point& pt, wsl::Point& w, wsl::Point& v) {
        wsl::Point v1 = w - pt;
        wsl::Point v2 = v - pt;
        wsl::Point t = w - v;

        double cd = fabs(v1.x() * v2.y() - v1.y() * v2.x());
        double nd = sqrt(t.x() * t.x() + t.y() * t.y());
        return cd / nd;
    }

    bool get_insect_point(wsl::Point& l1p1, wsl::Point& l1p2, wsl::Point& l2p1, wsl::Point& l2p2, wsl::Point& out) {
        point p1(l1p1.x(), l1p1.y()), p2(l1p2.x(), l1p2.y());
        point q1(l2p1.x(), l2p1.y()), q2(l2p2.x(), l2p2.y());
        segment s1(p1, p2), s2(q1, q2);

        std::deque<point> output;
        boost::geometry::intersection(s1, s2, output);
        
        int i = 0;
        BOOST_FOREACH(point const& p, output)
        {
            out.setx(p.x());
            out.sety(p.y());
        }

        return (i != 0);
    }

    int gbk_to_utf8(std::string& str)
    {
        char buf[256]={0};
        memset(buf, 0, 256);
        int utf8len = ::gbk_to_utf8(str.c_str(), str.length(), buf, 256, UCONV_INVCHAR_ERROR);

        if (utf8len <= 0) {
            return -1;
        } else {
            buf[utf8len] = 0;
            str = buf;
            return 0;
        }
    }

    int generate_date_seconds()
    {
        time_t seconds;
        time(&seconds);
        return seconds;
    }

    bool is_outdate(int time_day, int begin_time, int end_time)
    {
        int date_len = time_day * 24 * 60 * 60;
        double date_diff = difftime(end_time, begin_time);

        return date_diff > date_len;
    }

    struct VPoiDistancePair {
        double distance;
        VirtualPoi* lvpoi;
        VirtualPoi* rvpoi;
    };
    
    bool sort_dis_pair(VPoiDistancePair* lpair, VPoiDistancePair* rpair) {
        return lpair->distance < rpair->distance;
    }

    void collision_general(std::vector<VirtualPoi*>& vpois, double dis) {
        for (size_t i = 0; i < vpois.size(); i++) {
            VirtualPoi* apoi = vpois[i];
            if (apoi->is_valid == false) {
                continue;
            }

            for (size_t j = i; j < vpois.size(); j++) {
                if (i == j) {
                    continue;
                }
                VirtualPoi* bpoi = vpois[j];
                if (bpoi->is_valid == false) {
                    continue;
                }
                double cdis = distance_line(apoi->loc, bpoi->loc);
                if (cdis < dis) {
                    bpoi->is_valid = false;
                }
            }
        }
    }

    void check_dis(std::vector<VirtualPoi*>& vpois, double dis, int pre_level) {
        for (size_t i = 0; i < vpois.size(); i++) {
            VirtualPoi* apoi = vpois[i];
            if (apoi->is_valid == false) {
                continue;
            }

            if (apoi->from != pre_level) {
                continue;
            }

            for (size_t j = i; j < vpois.size(); j++) {
                if (i == j) {
                    continue;
                }
                VirtualPoi* bpoi = vpois[j];
                if (bpoi->is_valid == false) {
                    continue;
                }
                if (bpoi->from != pre_level) {
                    continue;
                }
                double cdis = distance_line(apoi->loc, bpoi->loc);
                if (cdis < dis) {
                    /* bpoi->is_valid = false; */
                    std::cout << "<check> find closed poi! at level: " << pre_level << std::endl;
                }
            }
        }
    }

    void collision_vpoi_with_level_normal(std::vector<VirtualPoi*>& vpois, double dis, int base_l, int increase_l, int rank, ShowType st) {
        std::vector<VirtualPoi*> pts;
        for (VirtualPoi* vpoi: vpois) {
            if (vpoi->is_valid == false) {
                continue;
            }

            if (vpoi->from == base_l) {
                pts.emplace_back(vpoi);  
            }
        }

        // 置位
        auto enable_vpoi = [&](VirtualPoi* vp) {
            vp->from = base_l + increase_l;
            vp->rank = rank;
            vp->show_type = st;
        };

        if (pts.size() < 1) {
            return;
        }

        if (pts.size() == 1) {
            enable_vpoi(pts[0]);
            return;
        }

        int obj_from = base_l + increase_l;
        for (size_t i = 0; i < pts.size(); ++i) {
            VirtualPoi *apoi = pts[i];
            for (size_t j = i; j < pts.size(); ++j) {
                if (j == i) { continue; }
                VirtualPoi *bpoi = pts[j];
                if (bpoi->from == obj_from) {
                    continue;
                }

                double cdis = distance_line(apoi->loc, bpoi->loc);
                if (cdis < dis) {
                    enable_vpoi(apoi);
                    enable_vpoi(bpoi);
                }
            }
        }
    }

    void collision_vpoi_with_level(std::vector<VirtualPoi*>& vpois, double dis, int base_l, int increase_l, int rank, ShowType st) {
        std::vector<wsl::Point> pts;
        std::unordered_map<size_t, VirtualPoi*> vpois_map;        
        for (VirtualPoi* vpoi: vpois) {
            if (vpoi->is_valid == false) {
                continue;
            }

            if (vpoi->from == base_l) {
                vpois_map[pts.size()] = vpoi;
                pts.emplace_back(vpoi->loc);  
            }
        }

        // 置位
        auto enable_vpoi = [&](VirtualPoi* vp) {
            vp->from = base_l + increase_l;
            vp->rank = rank;
            vp->show_type = st;
        };

        if (pts.size() < 1) {
            return;
        }

        if (pts.size() == 1) {
            enable_vpoi(vpois_map[0]);
            return;
        }

        std::vector<VPoiDistancePair*> dis_pairs;
        voronoi_diagram<double> vd;
        construct_voronoi(pts.begin(), pts.end(), &vd);

        unsigned int cell_index = 0;
        for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin();
            it != vd.cells().end(); ++it) {

            if (it->contains_point()) {
                if (it->source_category() == boost::polygon::SOURCE_CATEGORY_SINGLE_POINT) {
                    std::size_t index = it->source_index();
                    wsl::Point pt = pts[index];                    
                    const voronoi_edge<double>* edge = it->incident_edge();
                    do {
                        const voronoi_edge<double>* twin_edge = edge->twin();
                        const voronoi_cell<double>* twin_cell = twin_edge->cell();
                        if (twin_cell->contains_point()) {
                            if (twin_cell->source_category() == boost::polygon::SOURCE_CATEGORY_SINGLE_POINT) {
                                std::size_t sindex = twin_cell->source_index();
                                wsl::Point p = pts[sindex];
                                
                                VPoiDistancePair* dp = new VPoiDistancePair();
                                dp->distance = distance_line(pt, p);
                                dp->lvpoi = vpois_map[index];
                                dp->rvpoi = vpois_map[sindex];
                                dis_pairs.emplace_back(dp); 
                            }
                        }

                        edge = edge->next();
                    } while (edge != it->incident_edge());                    
                }
            }

            ++cell_index;
        }

        int obj_from = base_l + increase_l;
        for (size_t i = 0; i < dis_pairs.size(); i++) {
            VPoiDistancePair* dis_pair = dis_pairs[i];
            if (dis_pair->distance < dis) {
                if (dis_pair->lvpoi->from == obj_from && dis_pair->rvpoi->from == obj_from) {
                    continue;
                }
                enable_vpoi(dis_pair->lvpoi);
                enable_vpoi(dis_pair->rvpoi);
            }
        }

        for (VPoiDistancePair* dis_pair: dis_pairs) {
            delete dis_pair;
            dis_pair = NULL;
        }
    }

    void filter_near_vpoi(std::vector<VirtualPoi*>& vpois, double dis) {
        std::vector<wsl::Point> pts;
        std::unordered_map<size_t, VirtualPoi*> vpois_map;
        for (VirtualPoi* vpoi: vpois) {
            if (vpoi->is_valid == false) {
                continue;
            }

            vpois_map[pts.size()] = vpoi;
            pts.emplace_back(vpoi->loc);
        }

        if (pts.size() < 1) {
            return;
        }

        std::vector<VPoiDistancePair*> dis_pairs;
        voronoi_diagram<double> vd;
        construct_voronoi(pts.begin(), pts.end(), &vd);

        unsigned int cell_index = 0;
        for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin();
            it != vd.cells().end(); ++it) {

            if (it->contains_point()) {
                if (it->source_category() == boost::polygon::SOURCE_CATEGORY_SINGLE_POINT) {
                    std::size_t index = it->source_index();
                    wsl::Point pt = pts[index];
                    const voronoi_edge<double>* edge = it->incident_edge();
                    if (edge == nullptr) {
                        continue;
                    }
                    if (!vpois_map[index]->is_valid) {
                        continue;
                    }
                    do {
                        const voronoi_edge<double>* twin_edge = edge->twin();
                        const voronoi_cell<double>* twin_cell = twin_edge->cell();
                        if (twin_cell->contains_point()) {
                            if (twin_cell->source_category() == boost::polygon::SOURCE_CATEGORY_SINGLE_POINT) {
                                std::size_t sindex = twin_cell->source_index();
                                wsl::Point p = pts[sindex];

                                if (distance_line(pt, p) - dis < 0) {
                                    // std::cout << "<20 ,flase, distance_line = " << distance_line(pt, p) << ",dis = " << dis << ", cell_index = " << cell_index << std::endl;
                                    vpois_map[sindex]->is_valid = false;
                                }
                            }
                        }

                        edge = edge->next();
                    } while (edge != it->incident_edge());
                }
            }

            ++cell_index;
        }

    }

    void filter_near_tree(std::vector<VirtualPoi*>& vpois, std::vector<TreeData*>& trees, double len) {
        std::vector<wsl::Point> pts;
        std::size_t vpoi_len = vpois.size();
        for (VirtualPoi* vpoi: vpois) {
            pts.emplace_back(vpoi->loc);
        }
        for (TreeData* tree: trees) {
            pts.emplace_back(tree->loc);
        }

        voronoi_diagram<double> vd;
        construct_voronoi(pts.begin(), pts.end(), &vd);

        unsigned int cell_index = 0;
        for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin();
            it != vd.cells().end(); ++it) {

            if (it->contains_point()) {
                if (it->source_category() == boost::polygon::SOURCE_CATEGORY_SINGLE_POINT) {
                    std::size_t index = it->source_index();
                    if (index < vpoi_len) {
                        continue;
                    }

                    wsl::Point tree_pt = pts[index];

                    const voronoi_edge<double>* edge = it->incident_edge();
                    do {
                        const voronoi_edge<double>* twin_edge = edge->twin();
                        const voronoi_cell<double>* twin_cell = twin_edge->cell();
                        if (twin_cell->contains_point()) {
                            if (twin_cell->source_category() == boost::polygon::SOURCE_CATEGORY_SINGLE_POINT) {
                                std::size_t index = twin_cell->source_index();
                                if (index >= vpoi_len) {
                                    edge = edge->next();
                                    continue;
                                }

                                wsl::Point p = pts[index];
                                if (distance_line(tree_pt, p) < len) {
                                    VirtualPoi* vpoi = vpois[index];
                                    vpoi->is_valid = false;

                                    // std::cout << "invalid vpoi!" << std::endl;
                                }
                            }
                        }

                        edge = edge->next();
                    } while (edge != it->incident_edge());
                }
            }

            ++cell_index;
        }
    }

    #define LAYER_COUNT			9
    struct point_t {
        int64_t x;
        int64_t y;
    };
    const int each_level_max_scale[LAYER_COUNT] =  //每层数据最大比例尺,从0层起
                { 22, 17, 15, 13, 10, 9, 7, 5, 3 };
    struct block_id_t {
        int idx_x;
        int idx_y;
        int level;
        int blk_xy;
        point_t lb_pt;
        friend bool operator ==(const block_id_t& id1, const block_id_t& id2) {
            return (id1.level == id2.level && id1.idx_x == id2.idx_x
                    && id1.idx_y == id2.idx_y);
        }
        friend bool operator <(const block_id_t& id1, const block_id_t& id2) {
            return (id1.idx_x < id2.idx_x
                    || (id1.idx_x == id2.idx_x && id1.idx_y < id2.idx_y));
        }

        friend bool operator >(const block_id_t& id1, const block_id_t& id2) {
            return (id1.idx_x > id2.idx_x
                    || (id1.idx_x == id2.idx_x && id1.idx_y > id2.idx_y));
        }
    };

    int pt_in_which_blockid(const wsl::Point *pt, int level, const int split_xy,
        block_id_t &blkid) {
        if (split_xy <= 0) {
            return -1;
        }
        blkid.level = level;
        blkid.blk_xy = split_xy;
        if (pt->x() > 0) {
            blkid.idx_x = (int) ((pt->x()) / split_xy);
        } else {
            blkid.idx_x = (int) ((pt->x()) / split_xy) - 1;
        }
        if (pt->y() > 0) {
            blkid.idx_y = (int) ((pt->y()) / split_xy);
        } else {
            blkid.idx_y = (int) ((pt->y()) / split_xy) - 1;
        }
        blkid.lb_pt.x = blkid.blk_xy * blkid.idx_x;
        blkid.lb_pt.y = blkid.blk_xy * blkid.idx_y;

        return 0;
    }

    // get block id
    const int max_block[] =     {   8, 8, 8, 8, 8, 2, 1, 1, 1 };
    const int max_parcel[] =    {   8, 8, 8, 8, 8, 8, 4, 1, 1 };
    const int max_uinte[] =     {   8, 8, 8, 8, 8, 8, 8, 8, 4 };
    const int max_blockset[] = { 262144, 1048576, 2097152, 33554432, 33554432,33554432, 33554432,
                                33554432, 33554432 };
    // 每层数据网格实际跨度，从0层起  //// liujialin 21 0   20 1   19 2
    const int each_level_pixel[] =
            { 512, 2048, 4096, 65536, 65536,262144,  1048576, 4194304, 8388608};
    const int blockset_col[] =  { 256,64,32, 2, 2, 2, 2, 2, 2 };
    
    void block_id(int x, int y, int level, int &blockset, int &block, int &parcel,
        int &unite) {
        //加上第三象限
        x += (blockset_col[level] >> 1) * max_blockset[level];
        y += (blockset_col[level] >> 1) * max_blockset[level];
        int x_idx = x / each_level_pixel[level];
        int y_idx = y / each_level_pixel[level];
        int max_blockset_num = max_blockset[level];
        int max_block_num = max_block[level];
        int max_parcel_num = max_parcel[level];
        int max_blockunite_num = max_uinte[level];

        int level1_span = max_block_num * max_parcel_num * max_blockunite_num;
        int level2_span = max_parcel_num * max_blockunite_num;
        int level3_span = max_blockunite_num;

        int level1_idx_x = x / max_blockset[level];
        int level1_idx_y = y / max_blockset[level];

        int level1_offset_x = x_idx % level1_span;
        int level1_offset_y = y_idx % level1_span;

        int level2_idx_x = level1_offset_x / level2_span;
        int level2_idx_y = level1_offset_y / level2_span;
        int level2_offset_x = level1_offset_x % level2_span;
        int level2_offset_y = level1_offset_y % level2_span;

        int level3_idx_x = level2_offset_x / level3_span;
        int level3_idx_y = level2_offset_y / level3_span;
        int level3_offset_x = level2_offset_x % level3_span;
        int level3_offset_y = level2_offset_y % level3_span;
        blockset = level1_idx_x + level1_idx_y * blockset_col[level];
        //printf(" blolck :%d %d \n",level1_idx_x,level1_idx_y);
        block = level2_idx_x + level2_idx_y * max_block_num;
        parcel = level3_idx_x + level3_idx_y * max_parcel_num;
        unite = level3_offset_x + level3_offset_y * max_blockunite_num;
        //printf("set: %d bl :%d parcel:%d uint %d\n",blockset,block,parcel,unite);

    }

    std::string getblockid(const block_id_t &blkid) {
        int bs = 0;
        int b = 0;
        int bp = 0;
        int u = 0;

        block_id(blkid.blk_xy * blkid.idx_x,
                    blkid.blk_xy * blkid.idx_y, blkid.level, bs, b, bp,
                    u);

        char buf[64];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, 64, "1%02d%04d%02d%02d%02d", blkid.level, bs, b, bp, u);
        return std::string(buf);
    }

    inline int level_trans(int ilevel) {
        int olevel = 0;
        for (size_t i = 0; i < 8; i++) {
            if (ilevel < each_level_max_scale[i] && ilevel > each_level_max_scale[i + 1]) {
                olevel = i;
                break;
            }
        }
        return olevel;
    }

    void static_vpois(std::vector<VirtualPoi*>& vpois) {
        for (int level = 10; level <= each_level_max_scale[0]; level++) 
        {
            int blk_size = 1024;
            if (level < 18) 
            {
                blk_size = 1024 * pow(2.0, 18 - level);
            }

            std::map<block_id_t, size_t> levelgrids;
            for (size_t i = 0; i < vpois.size(); ++i)
            {
                VirtualPoi* vpoi = vpois[i];
                if (vpoi->is_valid == false) {
                    continue;
                }

                if (vpoi->from != level) {
                    continue;
                }

                block_id_t blkid;
                int ilevel = level_trans(level); 
                pt_in_which_blockid(&vpoi->loc, ilevel, blk_size, blkid);

                if (levelgrids.find(blkid) == levelgrids.end()) {
                    levelgrids[blkid] = 0;
                } else {
                    levelgrids[blkid]++;
                }
            }

            if (levelgrids.size() > 0) {
                std::cout << " vpois cnt at level: " << level << std::endl;
                for (auto i = levelgrids.begin(); i!= levelgrids.end(); ++i) {
                    size_t vpois_cnt = i->second;
                    std::cout << " grid: " << getblockid(i->first) << " , vpois cnt: " << vpois_cnt << std::endl;
                }
                std::cout << "----------------------" << std::endl;
            }
        }
    }
}
