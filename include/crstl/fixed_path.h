#pragma once

#include "crstl/utility/path_common.h"

#include "crstl/fixed_string.h"

crstl_module_export namespace crstl
{
	typedef path_base<fixed_string16> fixed_path16;
	typedef path_base<fixed_string32> fixed_path32;
	typedef path_base<fixed_string64> fixed_path64;
	typedef path_base<fixed_string128> fixed_path128;
	typedef path_base<fixed_string256> fixed_path256;
	typedef path_base<fixed_string512> fixed_path512;
	typedef path_base<fixed_string1024> fixed_path1024;
};