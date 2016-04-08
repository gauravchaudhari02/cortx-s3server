/*
 * COPYRIGHT 2015 SEAGATE LLC
 *
 * THIS DRAWING/DOCUMENT, ITS SPECIFICATIONS, AND THE DATA CONTAINED
 * HEREIN, ARE THE EXCLUSIVE PROPERTY OF SEAGATE TECHNOLOGY
 * LIMITED, ISSUED IN STRICT CONFIDENCE AND SHALL NOT, WITHOUT
 * THE PRIOR WRITTEN PERMISSION OF SEAGATE TECHNOLOGY LIMITED,
 * BE REPRODUCED, COPIED, OR DISCLOSED TO A THIRD PARTY, OR
 * USED FOR ANY PURPOSE WHATSOEVER, OR STORED IN A RETRIEVAL SYSTEM
 * EXCEPT AS ALLOWED BY THE TERMS OF SEAGATE LICENSES AND AGREEMENTS.
 *
 * YOU SHOULD HAVE RECEIVED A COPY OF SEAGATE'S LICENSE ALONG WITH
 * THIS RELEASE. IF NOT PLEASE CONTACT A SEAGATE REPRESENTATIVE
 * http://www.seagate.com/contact
 *
 * Original author:  Rajesh Nambiar <rajesh.nambiar@seagate.com>
 * Original creation date: 19-Nov-2015
 */

#include "gtest/gtest.h"
#include <functional>
#include "s3_auth_client.h"
#include <iostream>
#include <memory>
#include "mock_s3_asyncop_context_base.h"
#include "mock_s3_request_object.h"
#include "mock_s3_auth_client.h"
#include "mock_evhtp_wrapper.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::StrEq;
using ::testing::StrNe;
using ::testing::Return;
using ::testing::Mock;
using ::testing::InvokeWithoutArgs;

static void
dummy_request_cb(evhtp_request_t * req, void * arg) {
}

void dummy_function() {
  return;
}

class S3AuthBaseResponse {
public:
  virtual void auth_response_wrapper(evhtp_request_t * req, evbuf_t * buf, void * arg) = 0;
};

class S3AuthResponse : public S3AuthBaseResponse {
public:
  S3AuthResponse() {
    success_called = fail_called = false;
  }
  virtual void auth_response_wrapper(evhtp_request_t * req, evbuf_t * buf, void * arg) {
    on_auth_response(req, buf, arg);
  }

  void success_callback() {
    success_called = true;
  }

  void fail_callback() {
    fail_called = true;
  }

 bool success_called;
 bool fail_called;
};

class S3AuthClientOpContextTest : public testing::Test {
  protected:
    S3AuthClientOpContextTest() {
      evbase_t *evbase = event_base_new();
      evhtp_request_t *req = evhtp_request_new(NULL, evbase);
      EvhtpWrapper *evhtp_obj_ptr = new EvhtpWrapper();
      ptr_mock_request = std::make_shared<MockS3RequestObject> (req, evhtp_obj_ptr);
      EXPECT_CALL(*ptr_mock_request, get_evbase())
        .WillRepeatedly(Return(evbase));
      success_callback = NULL;
      failed_callback = NULL;
      p_authopctx = new S3AuthClientOpContext(ptr_mock_request, success_callback, failed_callback);
    }

  ~S3AuthClientOpContextTest() {
    delete p_authopctx;
  }

  std::shared_ptr<MockS3RequestObject> ptr_mock_request;
  std::function<void()> success_callback;
  std::function<void()> failed_callback;
  S3AuthClientOpContext *p_authopctx;
};

TEST_F(S3AuthClientOpContextTest, Constructor) {
  EXPECT_EQ(NULL, p_authopctx->auth_op_context);
  EXPECT_EQ(false, p_authopctx->has_auth_op_context);
}

TEST_F(S3AuthClientOpContextTest, InitAuthCtxNull) {
  evbase_t *evbase = NULL;
  EXPECT_CALL(*ptr_mock_request, get_evbase())
    .WillRepeatedly(Return(evbase));
  bool ret = p_authopctx->init_auth_op_ctx();
  EXPECT_EQ(false, ret);
}

TEST_F(S3AuthClientOpContextTest, InitAuthCtxValid) {
  bool ret = p_authopctx->init_auth_op_ctx();
  EXPECT_TRUE(ret == true);
}

TEST_F(S3AuthClientOpContextTest, GetAuthCtx) {
  p_authopctx->init_auth_op_ctx();
  struct s3_auth_op_context *p_ctx = p_authopctx->get_auth_op_ctx();
  EXPECT_TRUE(p_ctx != NULL);
}

TEST_F(S3AuthClientOpContextTest, CanParseAuthSuccessResponse) {
  std::string sample_response = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><AuthenticateUserResponse xmlns=\"https://iam.seagate.com/doc/2010-05-08/\"><AuthenticateUserResult><UserId>123</UserId><UserName>tester</UserName><AccountId>12345</AccountId><AccountName>s3_test</AccountName><SignatureSHA256>BSewvoSw/0og+hWR4I77NcWea24=</SignatureSHA256></AuthenticateUserResult><ResponseMetadata><RequestId>0000</RequestId></ResponseMetadata></AuthenticateUserResponse>";

  p_authopctx->set_auth_response_xml(sample_response.c_str(), true);

  EXPECT_TRUE(p_authopctx->is_auth_successful);
  EXPECT_STREQ("tester", p_authopctx->get_request()->get_user_name().c_str());
  EXPECT_STREQ("123", p_authopctx->get_request()->get_user_id().c_str());
  EXPECT_STREQ("s3_test", p_authopctx->get_request()->get_account_name().c_str());
  EXPECT_STREQ("12345", p_authopctx->get_request()->get_account_id().c_str());
}

TEST_F(S3AuthClientOpContextTest, CanHandleParseErrorInAuthSuccessResponse) {
  // Missing AccountId
  std::string sample_response = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><AuthenticateUserResponse xmlns=\"https://iam.seagate.com/doc/2010-05-08/\"><AuthenticateUserResult><UserId>123</UserId><UserName>tester</UserName><AccountName>s3_test</AccountName><SignatureSHA256>BSewvoSw/0og+hWR4I77NcWea24=</SignatureSHA256></AuthenticateUserResult><ResponseMetadata><RequestId>0000</RequestId></ResponseMetadata></AuthenticateUserResponse>";

  p_authopctx->set_auth_response_xml(sample_response.c_str(), true);

  EXPECT_FALSE(p_authopctx->is_auth_successful);
}

TEST_F(S3AuthClientOpContextTest, CanParseAuthErrorResponse) {
  std::string sample_response = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><Error xmlns=\"https://iam.seagate.com/doc/2010-05-08/\"><Code>SignatureDoesNotMatch</Code><Message>The request signature we calculated does not match the signature you provided. Check your AWS secret access key and signing method. For more information, see REST Authentication andSOAP Authentication for details.</Message><RequestId>0000</RequestId></Error>";

  p_authopctx->set_auth_response_xml(sample_response.c_str(), false);

  EXPECT_FALSE(p_authopctx->is_auth_successful);
  EXPECT_STREQ("SignatureDoesNotMatch", p_authopctx->get_error_res_obj()->get_code().c_str());
  EXPECT_STREQ("The request signature we calculated does not match the signature you provided. Check your AWS secret access key and signing method. For more information, see REST Authentication andSOAP Authentication for details.", p_authopctx->get_error_res_obj()->get_message().c_str());
  EXPECT_STREQ("0000", p_authopctx->get_error_res_obj()->get_request_id().c_str());
}

TEST_F(S3AuthClientOpContextTest, CanHandleParseErrorInAuthErrorResponse) {
  // Missing code
  std::string sample_response = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><Error xmlns=\"https://iam.seagate.com/doc/2010-05-08/\"><Message>The request signature we calculated does not match the signature you provided. Check your AWS secret access key and signing method. For more information, see REST Authentication andSOAP Authentication for details.</Message><RequestId>0000</RequestId></Error>";

  p_authopctx->set_auth_response_xml(sample_response.c_str(), false);

  EXPECT_FALSE(p_authopctx->get_error_res_obj()->isOK());
}