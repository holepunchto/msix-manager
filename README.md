# msix-manager

Install and update MSIX packages from JavaScript.

```
npm i msix-manager
```

## Usage

```js
const manager = new MSIXManager()

manager.addPackage('./App.msix')
  .on('progress', (progress) => {
    console.log('Progress', progress)
  })
  .then(
    () => {
      console.log('Finished')
    },
    (err) => {
      console.log('Failed', err)
    }
  )
```

## License

Apache-2.0
