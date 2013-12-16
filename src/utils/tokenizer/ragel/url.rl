%%{
  machine url;

  include tlds "tlds.rl";

  uchar = alnum | "$" | "-" | "_" | "." | "+" | "!" | "*" | "'" | "(" | ")" | "," | "%";
  xchar = uchar | ";" | "/" | "?" | ":" | "@" | "&" | "=";

  urlpath = '/' | '/' xchar* (alnum | "'" | ')');

  port = ':' digit+;

  ip = [0-9] | [1-9] [0-9] | '1' [0-9] [0-9] | '2' [0-4] [0-9] | '25' [0-5];
  hostnumber = ip '.' ip '.' ip '.' ip;
  topdomain = lower{2,} | upper{2,}; #tlds
  subdomain = alnum | alnum (alnum | "-")* alnum;
  hostname = (subdomain '.')+ topdomain;
  hostname_two_subdomains = (subdomain '.'){2,} topdomain;
  host = hostname | hostnumber;

  username = uchar+;
  password = uchar+;
  user = username (':' password)? '@';

  protocol = alpha{3,} '://'; #('ftp'i|'http'i|'https'i|'gopher'i|'mailto'i|'news'i|'nntp'i|'telnet'i|'wais'i|'file'i|'prosper'i) '://';

  url = protocol user? host port? urlpath? | user host port? urlpath? | host port urlpath? | host urlpath | hostname_two_subdomains;
  email = username '@' hostname;
}%%
