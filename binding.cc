#include <assert.h>
#include <bare.h>
#include <js.h>
#include <utf.h>
#include <uv.h>

#include "lib/windows-sdk.h"

struct msix_manager_t {
  PackageManager handle;
};

struct msix_manager_add_package_t {
  IAsyncOperationWithProgress<DeploymentResult, DeploymentProgress> handle;

  js_ref_t *ctx;
  js_threadsafe_function_t *on_progress;
  js_threadsafe_function_t *on_completed;
};

struct msix_manager_add_package_progress_t {
  DeploymentProgress handle;
};

static void
msix_manager__on_release(js_env_t *env, void *data, void *finalize_hint) {
  delete reinterpret_cast<msix_manager_t *>(data);
}

static js_value_t *
msix_manager_init(js_env_t *env, js_callback_info_t *info) {
  int err;

  auto manager = new msix_manager_t();

  js_value_t *result;
  err = js_create_external(env, manager, msix_manager__on_release, nullptr, &result);
  assert(err == 0);

  return result;
}

static void
msix_manager_add_package__on_progress(js_env_t *env, js_value_t *on_progress, void *context, void *data) {
  int err;

  auto req = reinterpret_cast<msix_manager_add_package_t *>(context);

  auto progress = reinterpret_cast<msix_manager_add_package_progress_t *>(data);

  js_handle_scope_t *scope;
  err = js_open_handle_scope(env, &scope);
  assert(err == 0);

  js_value_t *ctx;
  err = js_get_reference_value(env, req->ctx, &ctx);
  assert(err == 0);

  js_value_t *args[1];

  err = js_create_object(env, &args[0]);
  assert(err == 0);

#define V(name, n) \
  { \
    js_value_t *val; \
    err = js_create_uint32(env, n, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, args[0], name, val); \
    assert(err == 0); \
  }

  V("percentage", progress->handle.percentage);
  V("state", uint32_t(progress->handle.state));
#undef V

  err = js_call_function(env, ctx, on_progress, 1, args, nullptr);
  (void) err;

  err = js_close_handle_scope(env, scope);
  assert(err == 0);

  delete progress;
}

static void
msix_manager_add_package__on_completed(js_env_t *env, js_value_t *on_completed, void *context, void *data) {
  int err;

  auto req = reinterpret_cast<msix_manager_add_package_t *>(context);

  js_handle_scope_t *scope;
  err = js_open_handle_scope(env, &scope);
  assert(err == 0);

  js_value_t *ctx;
  err = js_get_reference_value(env, req->ctx, &ctx);
  assert(err == 0);

  err = js_unref_threadsafe_function(env, req->on_progress);
  assert(err == 0);

  err = js_unref_threadsafe_function(env, req->on_completed);
  assert(err == 0);

  err = js_release_threadsafe_function(req->on_progress, js_threadsafe_function_release);
  assert(err == 0);

  err = js_release_threadsafe_function(req->on_completed, js_threadsafe_function_release);
  assert(err == 0);

  err = js_delete_reference(env, req->ctx);
  assert(err == 0);

  js_value_t *args[1];

  auto result = req->handle.GetResults();

  if (req->handle.Status() == AsyncStatus::Error) {
    auto error = result.ErrorText();

    err = js_create_string_utf16le(env, reinterpret_cast<const utf16_t *>(error.data()), error.size(), &args[0]);
    assert(err == 0);
  } else {
    err = js_get_null(env, &args[0]);
    assert(err == 0);
  }

  err = js_call_function(env, ctx, on_completed, 1, args, nullptr);
  (void) err;

  err = js_close_handle_scope(env, scope);
  assert(err == 0);

  delete req;
}

static js_value_t *
msix_manager_add_package(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 5;
  js_value_t *argv[5];

  err = js_get_callback_info(env, info, &argc, argv, nullptr, nullptr);
  assert(err == 0);

  assert(argc == 5);

  msix_manager_t *manager;
  err = js_get_value_external(env, argv[0], (void **) &manager);
  assert(err == 0);

  size_t len;
  err = js_get_value_string_utf16le(env, argv[1], nullptr, 0, &len);
  assert(err == 0);

  len += 1 /* NULL */;

  std::vector<wchar_t> uri(len);
  err = js_get_value_string_utf16le(env, argv[1], reinterpret_cast<utf16_t *>(uri.data()), len, nullptr);
  assert(err == 0);

  auto req = new msix_manager_add_package_t();

  err = js_create_reference(env, argv[2], 1, &req->ctx);
  assert(err == 0);

  err = js_create_threadsafe_function(env, argv[3], 0, 1, nullptr, nullptr, req, msix_manager_add_package__on_progress, &req->on_progress);
  assert(err == 0);

  err = js_create_threadsafe_function(env, argv[4], 0, 1, nullptr, nullptr, req, msix_manager_add_package__on_completed, &req->on_completed);
  assert(err == 0);

  js_value_t *result;
  err = js_create_external(env, req, nullptr, nullptr, &result);
  assert(err == 0);

  AddPackageOptions options;

  options.DeferRegistrationWhenPackagesAreInUse(true);

  req->handle = manager->handle.AddPackageByUriAsync(Uri(hstring(uri.data(), len)), options);

  req->handle.Progress([=](auto &, auto &progress) {
    int err;
    err = js_call_threadsafe_function(req->on_progress, new msix_manager_add_package_progress_t{progress}, js_threadsafe_function_blocking);
    assert(err == 0);
  });

  req->handle.Completed([=](auto &, auto &) {
    int err;
    err = js_call_threadsafe_function(req->on_completed, nullptr, js_threadsafe_function_blocking);
    assert(err == 0);
  });

  return result;
}

static js_value_t *
msix_manager_exports(js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, nullptr, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("init", msix_manager_init)
  V("addPackage", msix_manager_add_package)
#undef V

  return exports;
}

BARE_MODULE(msix_manager, msix_manager_exports)
