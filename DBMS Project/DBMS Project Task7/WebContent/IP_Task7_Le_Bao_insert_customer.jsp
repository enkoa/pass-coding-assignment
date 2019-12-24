<%@ page language="java" contentType="text/html; charset=UTF-8"
pageEncoding="UTF-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Query Result</title>
</head>
    <body>
    <%@page import="ProjectTask7.IP_Task7_Le_Bao_DataHandler"%>
    <%@page import="java.sql.ResultSet"%>
    <%@page import="java.sql.Array"%>
    <%
    // The handler is the one in charge of establishing the connection.
    IP_Task7_Le_Bao_DataHandler handler = new IP_Task7_Le_Bao_DataHandler();

    // Get the attribute values passed from the input form.
    String cname = request.getParameter("cname");
    String address = request.getParameter("address");
    String cat = request.getParameter("category");

    /*
     * If the user hasn't filled out name, category, or address.
     */
    if (cname.equals("") || address.equals("") || cat.equals("")) {
        response.sendRedirect("IP_Task7_Le_Bao_insert_customer_form.jsp");
    } else {
        int category = Integer.parseInt(cat);

        // Now perform the query with the data from the form.
        boolean success = handler.addCustomer(cname, address, category);
        if (!success) { // Something went wrong
            %>
                <h2>There was a problem inserting into the table</h2>
            <%
        } else { // Confirm success to the user
            %>
            <h2>Customer:</h2>

            <ul>
                <li>Name: <%= cname %></li>
                <li>Address: <%= address %></li>
                <li>Category: <%= category %></li>
            </ul>

            <h2>Was successfully inserted.</h2>

            <a href="IP_Task7_Le_Bao_get_customer_form.jsp">Retrieve customer.</a><br>
            <a href="IP_Task7_Le_Bao_insert_customer_form.jsp">Insert customer.</a>
            <%
        }
    }
    %>
    </body>
</html>
