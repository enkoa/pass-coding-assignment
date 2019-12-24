<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title>Fill Customer Table</title>
    </head>
    <body>
        <h2>Add Customer to the Job-Shop Accounting Database System</h2>
        <!--
            Form for collecting user input for the customer record
            Upon form submission, IP_Task7_Le_Bao_insert_customer.jsp file will be invoked.
        -->
        <form action="IP_Task7_Le_Bao_insert_customer.jsp">
            <!-- The form organized in an HTML table for better clarity. -->
            <table border=1>
                <tr>
                    <th colspan="2">Enter the Customer data:</th>
                </tr>
                <tr>
                    <td>Customer name:</td>
                    <td><div style="text-align: center;">
                    <input type=text name=cname>
                    </div></td>
                </tr>
                <tr>
                    <td>Address:</td>
                    <td><div style="text-align: center;">
                    <input type=text name=address>
                    </div></td>
                </tr>
                <tr>
                    <td>Category(1-10):</td>
                    <td><div style="text-align: center;">
                    <input type=text name=category>
                    </div></td>
                </tr>
                <tr>
                    <td><div style="text-align: center;">
                    <input type=reset value=Clear>
                    </div></td>
                    <td><div style="text-align: center;">
                    <input type=submit value=Insert>
                    </div></td>
                </tr>
            </table>
        </form>
    </body>
</html>
