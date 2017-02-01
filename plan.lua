sctp = {
  --[[
    returns a userdata ("sock"), this will be the first parameter for every other sctp function
    version: should it be a number or a string?
  ]]
  socket(version),
  
  --[[
    sock: contains the address family. should I support IP version mixing?
    addr1: at least 1 address is mandatory
    ...: no limit for multihoming
  ]]
  bind(sock, addr1, ...),
  listen(),
  connect(),
  send(),
  recv(),
  setsockopt(),
  setnonblocking(),
  registerevents()
}