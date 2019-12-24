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
    String lb = request.getParameter("lb");
    String ub = request.getParameter("ub");
	
    /*
     * If the user hasn't filled out lower or upper bound.
     */
     
    if (ub.equals("") || lb.equals("")) {
        response.sendRedirect("IP_Task7_Le_Bao_insert_customer_form.jsp");   	
    } else {
    	
    	final ResultSet rs = handler.getCustomer(lb, ub);
    	
    	%>
    	<table cellspacing="2" cellpadding="2" border="1">
            <tr> <!-- The table headers row -->
              <td align="center">
                <h4>Customer name</h4>
              </td>
              <td align="center">
                <h4>Customer Address</h4>
              </td>
              <td align="center">
                <h4>Customer Category</h4>
              </td>
            </tr>	
    	<%
    	while(rs.next()) { // For each customer record returned...
            // Extract the attribute values for every row returned
            final String cname = rs.getString(1);
            final String caddress = rs.getString(2);
           	final String category = rs.getString(3);
            		
            out.println("<tr>"); // Start printing out the new table row
            out.println( // Print each attribute value
            		"<td align=\"center\">" + cname +
                    "</td><td align=\"center\"> " + caddress +
                    "</td><td align=\"center\"> " + category + "</td>");
            out.println("</tr>");
       }
    	%>
    	</table>
    	
    	<a href="IP_Task7_Le_Bao_insert_customer_form.jsp">Insert customer.</a><br>
    	<a href="IP_Task7_Le_Bao_get_customer_form.jsp">Retrieve customer.</a>
    	<%
    }   
    %>
    </body>
</html>
