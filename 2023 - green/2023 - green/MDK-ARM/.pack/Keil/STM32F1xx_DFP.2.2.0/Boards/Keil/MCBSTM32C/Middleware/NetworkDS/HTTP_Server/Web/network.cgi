t <html><head><title>Network Settings</title>
t <script language=JavaScript>
t function changeConfirm(f){
t  if(!confirm('Are you sure you want to change\nthe Network settings?')) return;
t  f.submit();
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Network Settings</h2>
t <p><font size="2">Here you can change the system <b>Network Settings</b>.
t  After you have changed the IP address, you need to change also the host IP address in 
t  you Internet browser to re-connect to target.<br><br>
t  This Form uses a <b>GET</b> method to send data to a Web server.</font></p>
t <form action=network.cgi method=get name=cgi>
t <input type=hidden value="net" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=30%>Item</th>
t  <th width=20%>IPv4</th>
t  <th width=50%>IPv6</th>
t </tr>
# Here begin data setting which is formatted in HTTP_Server_CGI.c module
t <tr>
t      <td><img src=pabb.gif>Link-local Address</td>
t      <td><input type=text name=l4 value=""   size=15 maxlength=15 disabled></td>
c a l6 <td><input type=text name=l6 value="%s" size=40 maxlength=40 disabled></td>
t </tr>
t <tr>
t      <td><img src=pabb.gif>IP Address</td>
c a i4 <td><input type=text name=i4 value="%s" size=15 maxlength=15></td>
c a i6 <td><input type=text name=i6 value="%s" size=40 maxlength=40></td>
t </tr>
t <tr>
t      <td><img src=pabb.gif>Network Mask</td>
c a m4 <td><input type=text name=m4 value="%s" size=15 maxlength=15></td>
t      <td><input type=text name=m6 value=""   size=40 maxlength=40 disabled></td>
t </tr>
t <tr>
t      <td><img src=pabb.gif>Default Gateway</td>
c a g4 <td><input type=text name=g4 value="%s" size=15 maxlength=15></td>
c a g6 <td><input type=text name=g6 value="%s" size=40 maxlength=40></td>
t </tr>
t <tr>
t      <td><img src=pabb.gif>Primary DNS Server</td>
c a p4 <td><input type=text name=p4 value="%s" size=15 maxlength=15></td>
c a p6 <td><input type=text name=p6 value="%s" size=40 maxlength=40></td>
t </tr>
t <tr>
t      <td><img src=pabb.gif>Secondary DNS Server</td>
c a s4 <td><input type=text name=s4 value="%s" size=15 maxlength=15></td>
c a s6 <td><input type=text name=s6 value="%s" size=40 maxlength=40></td>
t </tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=button name=set value="Change" onclick="changeConfirm(this.form)">
t <input type=reset value="Undo">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.

