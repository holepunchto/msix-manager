const EventEmitter = require('events')
const { pathToFileURL } = require('url')
const binding = require('#binding')

module.exports = class MSIXManager {
  constructor() {
    this._handle = binding.init()
  }

  addPackage(file) {
    return new MSIXManagerAddPackage(this, pathToFileURL(file).href)
  }
}

class MSIXManagerAddPackage {
  constructor(manager, uri) {
    this._manager = manager

    this._handle = binding.addPackage(manager._handle, uri, this)
  }
}
