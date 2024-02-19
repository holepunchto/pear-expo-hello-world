require('./runtime')

const RPC = require('tiny-buffer-rpc')
const ce = require('compact-encoding')

const rpc = new RPC(HelloBare.sendMessage)
HelloBare.onMessage = rpc.recv.bind(rpc)

rpc.register(0, {
  request: ce.string,
  response: ce.string,
  onrequest: message => message.split('').reverse().join('')
})

// keep the event loop alive
setInterval(() => {}, 2000)

// tell app we're ready
HelloBare.onReady()