/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 parser
*/
#pragma once

#include "resp2_buffer.h"

#include "resp2_generics.h"
#include "resp2_string.h"
#include "resp2_error.h"
#include "resp2_number.h"
#include "resp2_bulkstring.h"
#include "resp2_array.h"

Resp2_generics* resp2_parser(Resp2_buffer* buffer);