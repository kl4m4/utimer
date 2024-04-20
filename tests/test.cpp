#include <gtest/gtest.h>

extern "C" {
  #include "utimer/utimer.h"
}

int _timestamp = 0;

int mocked_get_timestamp() {
  return _timestamp;
}



struct mocked_utimer_callback {
  struct utimer_callback clbk;
  int call_counter;
};

void _mocked_clbk(const struct utimer_callback * const handler) {
    struct mocked_utimer_callback *instance = container_of(handler, struct mocked_utimer_callback, clbk);
    instance->call_counter++;
}

class UTimerFixture : public testing::Test {
 protected:
  struct mocked_utimer_callback callback_object;
  void SetUp() override {
    _timestamp = 0;
    callback_object.clbk = { .timeout = _mocked_clbk };
    callback_object.call_counter = 0;
    utimer_init(mocked_get_timestamp);
  }
  // void TearDown() override {}

  void SetFakeTimestamp(int value) { _timestamp = value;};
  void ResetCallback() {callback_object.call_counter = 0;};

};

TEST_F(UTimerFixture, SimpleCreation) {
  utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
  ASSERT_GT(uut, -1);
}

TEST_F(UTimerFixture, OutOfResources) {
  for(int i=0; i<UTIMER_MAX_TIMERS; i++) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
    ASSERT_GT(uut, -1);
  }
  utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
  ASSERT_EQ(uut, UTIMER_NO_RESOURCES);
}

TEST_F(UTimerFixture, ProcessUntilCallback) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
    utimer_start(uut);
    utimer_process();
    SetFakeTimestamp(5);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 0);
    SetFakeTimestamp(10);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 0);
    SetFakeTimestamp(11);
    utimer_process();
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 1);
}

TEST_F(UTimerFixture, LateStart) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
    SetFakeTimestamp(1000);
    utimer_start(uut);
    SetFakeTimestamp(1010);
    utimer_process();
    SetFakeTimestamp(1011);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 1);
}

TEST_F(UTimerFixture, NoCallbackWithoutTimerStart) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
    SetFakeTimestamp(9999999);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 0);
}

TEST_F(UTimerFixture, PeriodicalCallback) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, true);
    utimer_start(uut);

    SetFakeTimestamp(11);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 1);

    SetFakeTimestamp(21);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 2);

    SetFakeTimestamp(31);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 3);
}

TEST_F(UTimerFixture, NoCallbackAfterCancel) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
    utimer_start(uut);
    SetFakeTimestamp(2);
    utimer_process();
    utimer_cancel(uut);
    SetFakeTimestamp(9999999);
    utimer_process();
    ASSERT_EQ(callback_object.call_counter, 0);
}

TEST_F(UTimerFixture, UseAfterFree) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
    utimer_free(uut);
    auto retval = utimer_start(uut);
    ASSERT_EQ(retval, UTIMER_INSTANCE_ERROR);
}

TEST_F(UTimerFixture, DoubleFree) {
    utimer_handler uut = utimer_create_timer(10, &callback_object.clbk, false);
    utimer_free(uut);
    auto retval = utimer_free(uut);
    ASSERT_EQ(retval, UTIMER_INSTANCE_ERROR);

}