#include "middle.h"

template_t head_template[] = {
	{6, 'L', 0},
	{1, 'L', 0},
	{6, 'L', 0},
	{6, 'L', 0},
	{4, 'S', 8},
	{16, 'L', 0},
	{1, 'L', 0},
	{1, 'L', 0},
	{1, 'L', 0},
	{10, 'S', 16},
	{128, 'S', 136},
	{-1, -1, -1}
};

template_t login_req_template[] = {
	{10, 'S', 16},
	{16, 'S', 24},
	{16, 'L', 0},
	{8, 'S', 16},
	{10, 'L', 16},
	{1, 'S', 8},
	{-1, -1, -1}
};

template_t login_rsp_template[] = {
	{1, 'S', 8},
	{16, 'L', 0},
	{8, 'S', 16},
	{16, 'L', 0},
	{21, 'S', 24},
	{1, 'S', 8},
	{-1, -1, -1}
};

template_t biz_over_req_template[] = {
	{3, 'S', 8},
	{16, 'L', 0},
	{1, 'S', 8},
	{-1, -1, -1}
};

template_t biz_over_rsp_template[] = {
	{3, 'S', 8},
	{16, 'L', 0},
	{1, 'S', 8},
	{-1, -1, -1}
};	

template_t logout_req_template[] = {
	{1, 'S', 8},
	{64, 'S', 64},
	{-1, -1, -1}
};

template_t logout_rsp_template[] = {
	{-1, -1, -1}
};

template_t ping_req_template[] = {
	{16, 'S', 16},
	{8, 'S', 8},
	{-1, -1, -1}
};

template_t ping_rsp_template[] = {
	{16, 'S', 16},
	{8, 'S', 8},
	{-1, -1, -1}
};

template_t add_vol_req_template[] = {
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t add_vol_rsp_template[] = {
	{5, 'S', 8},
	{40, 'S', 40},
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t cut_vol_req_template[] = {
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t cut_vol_rsp_template[] = {
	{5, 'S', 8},
	{40, 'S', 40},
	{16, 'S', 16},
	{6, 'S', 8},
	{10, 'S', 16},
	{6, 'S', 8},
	{16, 'L', 0},
	{-1, -1, -1}
};

template_t trade_qry_req_template[] = {
	{16, 'S', 16},
	{-1, -1, -1}
};

template_t trade_qry_rsp_template[] = {
	{8, 'S', 16},
	{16, 'S', 16},
	{8, 'S', 16},
	{40, 'S', 40},
	{-1, -1, -1}
};
