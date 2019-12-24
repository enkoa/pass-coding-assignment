package ProjectTask7;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.DriverManager;
import java.sql.PreparedStatement;

public class IP_Task7_Le_Bao_DataHandler {
	private Connection conn;

	// Azure SQL connection credentials
	private String server = "le0058-sql-server.database.windows.net";
	private String database = "cs-dsa-4513-sql-db";
	private String username = "le0058";
	private String password = "Memelord1";

	// Resulting connection string
	final private String url = String.format(
			"jdbc:sqlserver://%s:1433;database=%s;user=%s;password=%s;encrypt=true;trustServerCertificate=false;hostNameInCertificate=*.database.windows.net;loginTimeout=30;",
			server, database, username, password);

	// Initialize and save the database connection
	private void getDBConnection() throws SQLException {
		if (conn != null) {
			return;
		}

		this.conn = DriverManager.getConnection(url);
	}
	
	public boolean addCustomer (String cname, String address, int category) throws SQLException {
		getDBConnection();
		
		final Statement statement = conn.createStatement();
				
		final String insertCustomer = "EXEC insert_customer '" + cname + "', '" + address + "', '" + category + "'";	
		
		return(statement.executeUpdate(insertCustomer) == 1);
	}
	
	public ResultSet getCustomer(String lb, String ub) throws SQLException {
		getDBConnection();
		
		final Statement statement = conn.createStatement();
				
		final String getCustomer = "EXEC get_customer '" + lb + "', '" + ub + "'";	
		
		return(statement.executeQuery(getCustomer));
	}
}
