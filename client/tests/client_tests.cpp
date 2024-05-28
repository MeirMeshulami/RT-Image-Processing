#include "JsonManager.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>


using namespace ::testing;



#pragma region Client

TEST(JSONSettings, loadConfigs) {
	nlohmann::json configs;
	JsonManager::ReadSettings(configs);
	std::string serverIP = configs["grpc_settings"]["camera_ip_address"];
	//std::cout << serverIP << std::endl;
	EXPECT_EQ("localhost", serverIP);
}



#pragma endregion




int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	return 0;
}



