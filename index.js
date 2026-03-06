const EventEmitter = require('events')
const { pathToFileURL } = require('url')
const binding = require('#binding')

module.exports = class MSIXManager {
  constructor() {
    this._handle = binding.init()
  }

  addPackage(file, opts = {}) {
    return new MSIXManagerAddPackage(this, pathToFileURL(file).href, opts)
  }
}

class MSIXManagerAddPackage extends EventEmitter {
  constructor(manager, uri, opts) {
    const { restartOnUpdate = false } = opts

    super()

    this._manager = manager

    this._completed = Promise.withResolvers()

    this._handle = binding.addPackage(
      manager._handle,
      uri,
      restartOnUpdate,
      this,
      this._onprogress,
      this._oncompleted
    )
  }

  then(resolve, reject) {
    return this._completed.promise.then(resolve, reject)
  }

  _onprogress(progress) {
    this.emit('progress', progress)
  }

  _oncompleted(err) {
    if (err) {
      this._completed.reject(new Error(err))
    } else {
      this._completed.resolve()
    }
  }
}
