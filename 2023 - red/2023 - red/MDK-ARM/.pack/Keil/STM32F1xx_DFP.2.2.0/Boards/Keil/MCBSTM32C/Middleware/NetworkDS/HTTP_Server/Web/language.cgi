t <html><head><title>Browser Language Preference</title></head>
i pg_header.inc
t <h2 align=center><br>Browser Language Preference</h2>
t <p><font size="2">You may use this information to create <b>Multi Language</b>
t  web pages.<br><br>
t  The language preferences can be set in Internet Explorer via 
t  <b>Tools / Internet Options... / Languages</b> and in Mozilla Firefox via <b>Tools /
t  Options... / Content / Languages</b>. You may change the setting in your browser
t  and reload this page again to check.<br><br>
t  Your browser is currently sending the following language preference:</font></p>
t <form action=index.htm method=post name=cgi>
t <input type=hidden value="lang" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
c e <tr><td><img src=pabb.gif>Browser Language</td><td><b>%s</b> [%s]</td>
t </tr></font></table><form>
i pg_footer.inc
. End of script must be closed with period.
