#include <gtest/gtest.h>
#include <hal-ml.h>


TEST(HAL_ML_PARAM, usecase)
{
  hal_ml_param_h param;

  EXPECT_EQ (hal_ml_param_create (&param), HAL_ML_ERROR_NONE);

  EXPECT_EQ (hal_ml_param_set (param, "some key", (void *) "some value"), HAL_ML_ERROR_NONE);

  EXPECT_EQ (hal_ml_param_destroy (param), HAL_ML_ERROR_NONE);
}

TEST (HAL_ML_PARAM, create_n)
{
  EXPECT_EQ (hal_ml_param_create (nullptr), HAL_ML_ERROR_INVALID_PARAMETER);
}

TEST (HAL_ML_PARAM, destroy_n)
{
  EXPECT_EQ (hal_ml_param_destroy (nullptr), HAL_ML_ERROR_INVALID_PARAMETER);
}

TEST (HAL_ML_PARAM, set_n)
{
  EXPECT_EQ (hal_ml_param_set (nullptr, "key", (void *) "value"), HAL_ML_ERROR_INVALID_PARAMETER);

  hal_ml_param_h param;
  EXPECT_EQ (hal_ml_param_create (&param), HAL_ML_ERROR_NONE);

  EXPECT_EQ (hal_ml_param_set (param, nullptr, (void *) "value"), HAL_ML_ERROR_INVALID_PARAMETER);
  EXPECT_EQ (hal_ml_param_set (param, "key", nullptr), HAL_ML_ERROR_INVALID_PARAMETER);

  EXPECT_EQ (hal_ml_param_destroy (param), HAL_ML_ERROR_NONE);
}

TEST (HAL_ML, create_n)
{
  EXPECT_EQ (hal_ml_create (nullptr, nullptr), HAL_ML_ERROR_INVALID_PARAMETER);
  EXPECT_EQ (hal_ml_create ("there_is_no_available_backend", nullptr), HAL_ML_ERROR_INVALID_PARAMETER);

  hal_ml_h ml;
  EXPECT_EQ (hal_ml_create (nullptr, &ml), HAL_ML_ERROR_INVALID_PARAMETER);
  EXPECT_NE (hal_ml_create ("there_is_no_available_backend", &ml), HAL_ML_ERROR_NONE);
}

TEST (HAL_ML, destroy_n)
{
  EXPECT_EQ (hal_ml_destroy (nullptr), HAL_ML_ERROR_INVALID_PARAMETER);
}

TEST (HAL_ML, request_n)
{
  EXPECT_EQ (hal_ml_request (nullptr, "some_request", nullptr), HAL_ML_ERROR_INVALID_PARAMETER);

  hal_ml_param_h param;
  EXPECT_EQ (hal_ml_param_create (&param), HAL_ML_ERROR_NONE);
  EXPECT_EQ (hal_ml_request (nullptr, "some_request", param), HAL_ML_ERROR_INVALID_PARAMETER);
  EXPECT_EQ (hal_ml_param_destroy (param), HAL_ML_ERROR_NONE);
}

int main (int argc, char *argv[])
{
  int ret = -1;

  try {
    testing::InitGoogleTest (&argc, argv);
  } catch(...) {
    std::cout << "Exception occurred." << std::endl;
  }

  try {
    ret = RUN_ALL_TESTS ();
  } catch (const ::testing::internal::GoogleTestFailureException& e) {
    ret = -1;
    std::cout << "GoogleTestFailureException was thrown:" << e.what () << std::endl;
  }

  return ret;
}
