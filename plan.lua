sctp = {
  --[[
    returns a userdata (Sctp::Socket)
  ]]
  socket(),
  
  --[[
    Calls bind() and sctp_bindx() internally. No IP version mixing
    sock: contains the address family.
    port: every given address will use this port number
    addr1: at least 1 address is mandatory
    ...: no limit for multihoming
  ]]
  bind(sock, port, addr1, ...),
  listen(),
  accept(),
  
  --[[
    Should it be connect or connectx?
  ]]
  connect(),
  
  --[[
    Only allow non-depcrecated functions. For now at least
  ]]
  send(),
  recv(),
  setsockopt(),
  setnonblocking(),
  registerevents(),
  
  --[[
    Should also handle sctp_free[p|l]addrs() internally
  ]]
  getpaddr(), getladdr()
}