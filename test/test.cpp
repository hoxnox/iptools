/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160516 09:09:55
 *
 * @brief iptools test launcher.*/

// Google Testing Framework
#include <gtest/gtest.h>

// test cases
#include "test_cidr_v4.hpp"
#include "test_cidr_v6.hpp"
#include "test_lpfst.hpp"
#include "test_lpfst_v6.hpp"

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


