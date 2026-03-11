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

static js_value_t *
msix_manager_add_package(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];
  err = js_get_callback_info(env, info, &argc, argv, nullptr, nullptr);
  assert(err == 0);

  assert(argc == 3);

  msix_manager_t *manager;
  err = js_get_value_external(env, argv[0], (void **) &manager);
  assert(err == 0);

  size_t len;
  err = js_get_value_string_utf16le(env, argv[1], nullptr, 0, &len);
  assert(err == 0);

  len += 1;
  std::vector<wchar_t> uri(len);
  err = js_get_value_string_utf16le(env, argv[1], reinterpret_cast<utf16_t *>(uri.data()), len, nullptr);
  assert(err == 0);

  auto req = new msix_manager_add_package_t();

  err = js_create_reference(env, argv[2], 1, &req->ctx);
  assert(err == 0);

  js_value_t *result;
  err = js_create_external(env, req, nullptr, nullptr, &result);
  assert(err == 0);

  AddPackageOptions options;
  options.DeferRegistrationWhenPackagesAreInUse(true);

  req->handle = manager->handle.AddPackageByUriAsync(Uri(hstring(uri.data(), len)), options);

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
