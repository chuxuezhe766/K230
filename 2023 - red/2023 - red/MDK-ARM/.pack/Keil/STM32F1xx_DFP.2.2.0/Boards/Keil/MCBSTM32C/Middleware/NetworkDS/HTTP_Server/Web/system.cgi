t <html><head><title>System Settings</title>
t <script language=JavaScript>
t function changeConfirm(f){
t  if(!confirm('Are you sure you want\nto change the password?')) return;
t  f.submit();
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>System Settings</h2>
t <p><font size="2">This page allows you to change the system
t  <b>Password</b>, for the username <b>admin</b>. Default <b>realm</b>, 
t  <b>user</b> and <b>password</b> can be set in configuraton file.<br><br>
t  This Form uses a <b>POST</b> method to send data to a Web server.</font></p>
t <form action=index.htm method=post name=cgi>
t <input type=hidden value="sys" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
c d 1 <tr><td><img src=pabb.gif>Authentication</TD><TD><b>%s</b></td></tr>
t <tr><td><img src=pabb.gif>Password for user 'admin'</td>
c d 2 <td><input type=password name=pw0 size=10 maxlength=10 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Retype your password</td>
c d 2 <td><input type=password name=pw2 size=10 maxlength=10 value="%s"></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=button name=set value="Change" onclick="changeConfirm(this.form)">
t <input type=reset value="Undo">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
