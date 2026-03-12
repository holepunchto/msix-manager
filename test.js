const test = require('brittle')
const path = require('path')
const MSIXManager = require('.')

const fixtures = path.join(__dirname, 'test', 'fixtures')

test('basic', async (t) => {
  const manager = new MSIXManager()

  const req = manager.addPackage(path.join(fixtures, 'basic.msix'), { allowUnsigned: true })

  req.on('progress', (progress) => {
    t.comment(progress)
  })

  await req
})
