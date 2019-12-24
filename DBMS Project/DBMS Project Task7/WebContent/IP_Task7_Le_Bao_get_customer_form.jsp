<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>Get From Customer Table</title>
    </head>
    <body>
        <h2>Get Customer from the Job-Shop Accounting Database System</h2>
        <!--
            Form for collecting user input to get the customer record
            Upon form submission, IP_Task7_Le_Bao_get_customer.jsp file will be invoked.
        -->
        <form action="IP_Task7_Le_Bao_get_customer.jsp">
            <!-- The form organized in an HTML table for better clarity. -->
            <table border=1>
                <tr>
                    <th colspan="2">Enter category lower and upper bound:</th>
                </tr>
                <tr>
                    <td>Category lower bound(1-10):</td>
                    <td><div style="text-align: center;">
                    <input type=text name=lb>
                    </div></td>
                </tr>
                <tr>
                    <td>Category upper bound(1-10, can be the same):</td>
                    <td><div style="text-align: center;">
                    <input type=text name=ub>
                    </div></td>
                </tr>
                <tr>
                    <td><div style="text-align: center;">
                    <input type=reset value=Clear>
                    </div></td>
                    <td><div style="text-align: center;">
                    <input type=submit value=Get>
                    </div></td>
                </tr>
            </table>
        </form>
    </body>
</html>
