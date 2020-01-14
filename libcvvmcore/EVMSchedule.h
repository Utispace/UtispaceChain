

#pragma once

#include <libcvvmcore/Instruction.h>
#include <libdevcore/Common.h>

namespace dev
{
namespace eth
{

struct EVMSchedule
{
	EVMSchedule(): tierStepGas(std::array<unsigned, 8>{{0, 2, 3, 5, 8, 10, 20, 0}}) {}
	EVMSchedule(bool _efcd, bool _hdc, unsigned const& _txCreateGas): exceptionalFailedCodeDeposit(_efcd), haveDelegateCall(_hdc), tierStepGas(std::array<unsigned, 8>{{0, 2, 3, 5, 8, 10, 20, 0}}), txCreateGas(_txCreateGas) {}
	bool exceptionalFailedCodeDeposit = true;
	bool haveDelegateCall = true;
	bool eip150Mode = false;
	bool eip158Mode = false;
	unsigned stackLimit = 1024;
	std::array<unsigned, 8> tierStepGas;
	unsigned expGas = 10;
	unsigned expByteGas = 10;
	unsigned sha3Gas = 30;
	unsigned sha3WordGas = 6;
	unsigned sloadGas = 50;
	unsigned sstoreSetGas = 20000;
	unsigned sstoreResetGas = 5000;
	unsigned sstoreRefundGas = 15000;
	unsigned jumpdestGas = 1;
	unsigned logGas = 375;
	unsigned logDataGas = 8;
	unsigned logTopicGas = 375;
	unsigned createGas = 32000;
	unsigned callGas = 40;
	unsigned callStipend = 2300;
	unsigned callValueTransferGas = 9000;
	unsigned callNewAccountGas = 25000;
	unsigned suicideRefundGas = 24000;
	unsigned memoryGas = 3;
	unsigned quadCoeffDiv = 512;
	unsigned createDataGas = 200;
	unsigned txGas = 21000;
	unsigned txCreateGas = 53000;
	unsigned txDataZeroGas = 4;
	unsigned txDataNonZeroGas = 68;
	unsigned copyGas = 3;

	unsigned extcodesizeGas = 20;
	unsigned extcodecopyGas = 20;
	unsigned balanceGas = 20;
	unsigned suicideGas = 0;
	unsigned maxCodeSize = unsigned(-1);

	bool staticCallDepthLimit() const { return !eip150Mode; }
	bool suicideChargesNewAccountGas() const { return eip150Mode; }
	bool emptinessIsNonexistence() const { return eip158Mode; }
	bool zeroValueTransferChargesNewAccountGas() const { return !eip158Mode; }
};

static const EVMSchedule DefaultSchedule = EVMSchedule();
static const EVMSchedule FrontierSchedule = EVMSchedule(false, false, 21000);
static const EVMSchedule HomesteadSchedule = EVMSchedule(true, true, 53000);

static const EVMSchedule EIP150Schedule = []
{
	EVMSchedule schedule = HomesteadSchedule;
	schedule.eip150Mode = true;
	schedule.extcodesizeGas = 700;
	schedule.extcodecopyGas = 700;
	schedule.balanceGas = 400;
	schedule.sloadGas = 200;
	schedule.callGas = 700;
	schedule.suicideGas = 5000;
	schedule.maxCodeSize = 0x6000;
	return schedule;
}();

static const EVMSchedule EIP158Schedule = []
{
	EVMSchedule schedule = EIP150Schedule;
	schedule.expByteGas = 50;
	schedule.eip158Mode = true;
	return schedule;
}();

}
}
