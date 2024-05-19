function FindProxyForURL(url, host) {

    if (shExpMatch(host, '*.test1.com')) { return "PROXY 192.168.18.193:1081"; }

    if (shExpMatch(host, '*.test2.com')) { return "HTTPS 192.168.18.193"; }
    
    if (shExpMatch(host, '*.test3.com')) { return "PROXY 192.168.18.193; PROXY 192.168.18.194:100;"; }
    
    if (shExpMatch(host, '*.test4.com')) { return "HTTPS 192.168.18.193; HTTPS 192.168.18.194:102; PROXY 192.168.18.195:103"; }

    return 'DIRECT';
}
