#include "CInputParameter.h"

CInputParameter::CInputParameter()
{
	CIP_Input_Data input_data = {
		3.5f, 4.0f, 0.8f, 0.38f, 0.4f, 0.3f, 4.2f, 1.5f, 1.0f, 2.0f, 0.15f, 1.5f, 1.5f, 0.05f,
		2.0f, 0.3f, 0.5f, 0.6f, 0.2f, 2.0f, 0.2f, 0.5f, 1.0f,
		3.0f, 0.1f, 3.0f, 1, 3,
		5.0f, 2.0f,
		4.0f, 10.0f, 2.0f, 2.0f,
		0.0f, 8.0f, 2.0f
	};

	// プレイヤージャンプした瞬間の横の速度
	input_data.mf_player_horizontal_jump_power = 1.7f;

	// プレイヤーの走り出しモーションの時間
	input_data.mf_player_idle_to_run_loop_time = 0.1f;

	// プレイヤーの走り終わりモーションの時間
	input_data.mf_player_run_to_idle_loop_time = 0.2f;

	// プレイヤーのジャンプ開始モーションの時間
	input_data.mf_player_jump_up_loop_time = 0.1f;

	this->data = new CIP_Data(input_data);
}

CInputParameter::~CInputParameter()
{
	if (this->data != nullptr) {
		delete this->data;
	}
}

CIP_Data CInputParameter::GetCIP_Data()
{
	return *this->data;
}

CIP_Data::CIP_Data(CIP_Input_Data data) : M_DATA(data)
{
}
