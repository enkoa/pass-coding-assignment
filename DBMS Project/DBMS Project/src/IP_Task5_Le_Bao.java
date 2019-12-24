import java.sql.Connection;
import java.sql.Statement;
import java.sql.Types;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;


/**
 *
 * @author 
 */
public class IP_Task5_Le_Bao {

    /**
     * @param args the command line arguments
     */
	
	
	
    public static void main(String[] args) {
        // Connect to database
        final String hostName = "le0058-sql-server.database.windows.net";
        final String dbName = "cs-dsa-4513-sql-db";
        final String user = "le0058";
        final String password = "Memelord1";
        final String url = String.format("jdbc:sqlserver://%s:1433;database=%s;user=%s;password=%s;encrypt=true;hostNameInCertificate=*.database.windows.net;loginTimeout=30;", hostName, dbName, user, password);
        
        try (final Connection connection = DriverManager.getConnection(url)) {
            final String schema = connection.getSchema();
            //System.out.println("Successful connection - Schema: " + schema);
            //System.out.println();
            
            Scanner input = new Scanner(System.in);
            int choice;
            
            while (true) {
            	System.out.println("WELCOME TO THE JOB-SHOP ACCOUNTING DATABASE SYSTEM");
            	System.out.println("Choice 1: Enter a new customer");
            	System.out.println("Choice 2: Enter a new department");
            	System.out.println("Choice 3: Enter  a  new  assembly  with  its  customer-name,  assembly-details,  assembly-id,  and  date-ordered");
            	System.out.println("Choice 4: Enter a new process-id and its department together with its type and information relevant to the type");
            	System.out.println("Choice 5: Create a new account and associate it with the process, assembly, or department to which it is applicable");
            	System.out.println("Choice 6: Enter  a  new  job,  given  its  job-no,  assembly-id,  process-id,  and  date  the  job  commenced");
            	System.out.println("Choice 7: At the completion of a job, enter the date it completed and the information relevant to the type of job");
            	System.out.println("Choice 8: Enter  a  transaction-no  and  its  sup-cost  and update  all  the  costs  (details) of  the  affected accounts by adding sup-cost to their current values of details");
            	System.out.println("Choice 9: Retrieve the cost incurred on an assembly-id");
            	System.out.println("Choice 10: Retrieve the total labor time within a department for jobs completed in the department during a given date");
            	System.out.println("Choice 11: Retrieve  the  processes  through  which  a  given  assembly-id  has  passed  so  far  (in  date-commenced order) and the department responsible for each process");
            	System.out.println("Choice 12: Retrieve  the  jobs  (together  with  their  type  information  and  assembly-id)  completed  during  a given date in a given department");
            	System.out.println("Choice 13: Retrieve the customers (in name order) whose category is in a givenrange");
            	System.out.println("Choice 14: Delete all cut-jobs whose job-no is in a givenrange ");
            	System.out.println("Choice 15: Change the color of a given paint job");
            	System.out.println("Choice 16: Import: enter new customers from a data file until the file is empty");
            	System.out.println("Choice 17: Export: Retrieve the customers (in name order) whose category is in a given range and output them to a data file instead of screen");
            	System.out.println("Choice 18: Quit");
            	System.out.print("Enter choices 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18: ");
            	
            	// input
            	choice = input.nextInt();
            	
            	// input evaluation
            	if (choice == 18) {
            		break;
            	} else if (choice == 1) {
            		queryOne(connection);
            	} else if (choice == 2) {
            		queryTwo(connection);
            	} else if (choice == 3) {
            		queryThree(connection);
            	} else if (choice == 4) {
            		queryFour(connection);
            	} else if (choice == 5) {
            		queryFive(connection);
            	} else if (choice == 6) {
            		querySix(connection);
            	} else if (choice == 7) {
            		querySeven(connection);
            	} else if (choice == 8) {
            		queryEight(connection);
            	} else if (choice == 9) {
            		queryNine(connection);
            	} else if (choice == 10) {
            		queryTen(connection);
            	} else if (choice == 11) {
            		queryEleven(connection);
            	} else if (choice == 12) {
            		queryTwelve(connection);
            	} else if (choice == 13) {
            		queryThirteen(connection);
            	} else if (choice == 14) {
            		queryFourteen(connection);
            	} else if (choice == 15) {
            		queryFifteen(connection);
            	} else if (choice == 16) {
            		importCustomer(connection);
            	} else if (choice == 17) {
            		exportCustomer(connection);
            	} else {
            		System.out.println("Choices not in range.");
            	}
            	
            }          
        } catch (Exception ae) {
        	System.out.println(ae);
        }
        System.out.println("Fin");
    }

    
    
    public static void queryOne(Connection connection) {
    	String name, address;
    	int category;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
        	System.out.print("Enter customer name: ");
        	name = input.nextLine();
        	System.out.print("Enter customer address: ");
        	address = input.nextLine();
        	System.out.print("Enter customer category: ");
        	category = input.nextInt();
    		
        	// execute procedure
    		final String insertCustomer =  "EXEC insert_customer '" + name + "', '" + address + "', '" + category + "'";
    		final ResultSet rs = statement.executeQuery(insertCustomer);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    
    public static void queryTwo(Connection connection) {
    	String ddata;
    	int dnum;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter the Department number: ");
        	dnum = input.nextInt();
        	ddata = input.nextLine();
        	System.out.print("Enter the Department data: ");
        	ddata = input.nextLine();
    		
        	// execute procedure
    		final String insertDepartment =  "EXEC insert_department '" + dnum + "', '" + ddata + "'";
    		final ResultSet rs = statement.executeQuery(insertDepartment);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    public static void queryThree(Connection connection) {
    	String cname, adetail, date;
    	int month, day, year, aid;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter new Assembly ID: ");
        	aid = input.nextInt();
        	cname = input.nextLine();
        	System.out.print("Enter the customer name who order the assembly: ");
        	cname = input.nextLine();
        	System.out.print("Enter Assembly detail: ");
        	adetail = input.nextLine();
        	System.out.print("Enter day order(DD): ");
        	day = input.nextInt();
        	System.out.print("Enter month order(MM): ");
        	month = input.nextInt();
        	System.out.print("Enter year order(YYYY): ");
        	year = input.nextInt();
        	
        	// make date
        	date = year + "-" + month + "-" + day;	
        	
        	// execute procedure
    		final String insertAssembly =  "EXEC insert_assembly '" + aid + "', '" + date + "', '" + adetail + "', '" + cname + "'";
    		final ResultSet rs = statement.executeQuery(insertAssembly);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    	
    }

    public static void queryFour(Connection connection) {
    	int pdata, pid, dept;
    	String info2 = "";
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter new process ID: ");
        	pid = input.nextInt();
        	System.out.print("Enter department number for this process: ");
        	dept = input.nextInt();
        	System.out.print("Enter type of process{Cut(1), Paint(2), Fit(3)}: ");
        	pdata = input.nextInt();
        	
        	info2 = input.nextLine();	// clear the end line
    		
        	// extra information depending on type of process
    		if (pdata == 1) {
        		System.out.print("Enter machine type: ");
            	info2 = input.nextLine();
        	} else if (pdata == 2) {
        		System.out.print("Enter paint method: ");
            	info2 = input.nextLine();
        	} else if (pdata == 3) {
        		// nothing
        	} else {
        		System.out.println("Invalid choice");
        		return;
        	}
    		
    		// execute procedure
    		final String insertProcess = "EXEC insert_process '" + pid + "', '" + pdata + "', '" + dept + "', '" + info2 + "'"; 
    		final ResultSet rs = statement.executeQuery(insertProcess);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}	
    }
    public static void queryFive(Connection connection) {
    	String date;
    	int assoc, pk, day, month, year, account;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter new Account number: ");
    		account = input.nextInt();
    		System.out.print("Is this for Assembly(1), Process(2), or Department(3)? ");
    		assoc = input.nextInt();
    		
    		// case prompt for assembly, process, and department
    		if (assoc == 1) {
    			System.out.print("Enter Assembly ID for this account to track: ");
    			pk = input.nextInt();
    		} else if (assoc == 2) {
    			System.out.print("Enter Process ID for this account to track: ");
    			pk = input.nextInt();
    		} else if (assoc == 3) {
    			System.out.print("Enter Department number for this account to track: ");
    			pk = input.nextInt();
    		} else {
    			System.out.println("Invalid choice");
        		return;
    		}
    		
    		// ask for date
    		System.out.print("Enter day made(DD): ");
        	day = input.nextInt();
        	System.out.print("Enter month made(MM): ");
        	month = input.nextInt();
        	System.out.print("Enter year made(YYYY): ");
        	year = input.nextInt();
        	
        	// make date
        	date = year + "-" + month + "-" + day;	
    		
        	//execute query
    		final String insertAccount = "EXEC insert_accounts '" + account + "', '" + assoc + "', '" + pk + "', '" + date + "'";
    		final ResultSet rs = statement.executeQuery(insertAccount);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }

    public static void querySix(Connection connection) {
    	String date;
    	int day, month, year, job, aid, pid;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter a Job number: ");
    		job = input.nextInt();
    		System.out.print("Enter associated Assembly ID: ");
    		aid = input.nextInt();
    		System.out.print("Enter associated Process ID: ");
    		pid = input.nextInt();		
    		System.out.print("Enter day commenced(DD): ");
        	day = input.nextInt();
        	System.out.print("Enter month commenced(MM): ");
        	month = input.nextInt();
        	System.out.print("Enter year commenced(YYYY): ");
        	year = input.nextInt();
        	
        	// make date
        	date = year + "-" + month + "-" + day;	
    		
        	//execute query
    		final String insertJob = "EXEC insert_job '" + job + "', '" + aid + "', '" + pid + "', '" + date + "'";
    		final ResultSet rs = statement.executeQuery(insertJob);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    public static void querySeven(Connection connection) {
    	int job, day, month, year, labor;
    	String date, info1 = "", info2 = "";
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter a Job number: ");
    		job = input.nextInt();	
    		System.out.print("Enter day completed(DD): ");
        	day = input.nextInt();
        	System.out.print("Enter month completed(MM): ");
        	month = input.nextInt();
        	System.out.print("Enter year completed(YYYY): ");
        	year = input.nextInt();
        	
        	// make date
        	date = year + "-" + month + "-" + day;	
    		
        	// find and update table and retrieve info for information
    		final String insertJob = "EXEC insert_job_done '" + job + "', '" + date + "'";
    		final ResultSet rs = statement.executeQuery(insertJob);
    		
    		// check if table with the job number exist
    		if (!rs.next()) {
    			System.out.println("Job number does not exist");
    			return;
    		}
    		
    		// get the job type
    		String type = input.nextLine();
    		type = rs.getString(1);
    		
    		// get the other information depending on job type
    		if (type.equals("Cut")) {
    			System.out.print("Enter machine time(minutes): ");
    			info1 = input.nextLine();
    			System.out.print("Enter materials: ");
    			info2 = input.nextLine();
    		} else if (type.equals("Paint")) {
    			System.out.print("Enter Color: ");
    			info1 = input.nextLine();
    			System.out.print("Enter Volume(Litres): ");
    			info2 = input.nextLine();
    		} else if (type.equals("Fit")) {
    			// nothing
    		} else {
    			System.out.println("Table malfunction");
    			return;
    		}
    		
    		// labor time
    		System.out.print("Enter labor time(minutes): ");
    		labor = input.nextInt();
    		
    		// execute procedure
    		final String updateInfo = "EXEC update_job_info '" + job + "', '" + type + "', '" + labor + "', '" + info1 + "', '" + info2 + "'"; 
    		final ResultSet rs2 = statement.executeQuery(updateInfo);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }

    public static void queryEight(Connection connection) {
    	int trans, job;
    	float cost;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter transaction number: ");
    		trans = input.nextInt();
    		System.out.print("Enter a job number for this transaction: ");
    		job = input.nextInt();
    		System.out.print("Enter the transaction cost $: ");
    		cost = input.nextFloat();
    		
    		// execute procedure
    		final String insertTrans = "EXEC insert_trans '" + trans + "', '" + job + "', '" + cost + "'";
    		final ResultSet rs = statement.executeQuery(insertTrans);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    public static void queryNine(Connection connection) {
    	int aid;
    	
    	Scanner input = new Scanner (System.in);
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter Assembly ID to retrieve cost: ");
    		aid = input.nextInt();
    		
    		// execute procedure
    		final String assemblyCost = "EXEC assembly_cost '" + aid + "'";
    		final ResultSet rs = statement.executeQuery(assemblyCost);
    		
    		// print cost
    		if (rs.next()) {
    			System.out.println("\n\tAssembly ID: " + aid + "\n\tCost for this Assembly: $" + rs.getFloat(1) + "\n");
    		}
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    	
    }
    public static void queryTen(Connection connection) {
    	int dnum, day, month, year;
    	String date;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter department to retrieve labor time from: ");
    		dnum = input.nextInt();
    		System.out.print("Enter day(DD): ");
    		day = input.nextInt();
    		System.out.print("Enter month(MM): ");
    		month = input.nextInt();
    		System.out.print("Enter year(YYYY): ");
    		year = input.nextInt();
    		
    		// make date
    		date = year + "-" + month + "-" + day;
    		
    		// execute procedure
    		final String laborTime = "EXEC labor_time '" + dnum + "', '" + date + "'";
    		PreparedStatement pst = connection.prepareStatement(laborTime);
    		final ResultSet rs = pst.executeQuery();
    		
    		// print total labor time  
    		if (rs.next()) {
    			System.out.println("\n\tTotal Labor time on " + date +  " for Department " 
    					+ dnum + ": " + rs.getString(1) + " minutes\n");
    		}
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    	
    }
    public static void queryEleven(Connection connection) {
    	int aid;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter assembly ID: ");
    		aid = input.nextInt();
    		
    		// execute procedure
    		final String assemblyPass = "EXEC assembly_pass '" + aid + "'";
    		final ResultSet rs = statement.executeQuery(assemblyPass);
    		
    		// print out all process that have that assembly passed and department
    		System.out.println();
    		while (rs.next()) {
    			System.out.println(String.format("\tDate: %s | Process: %s | Department: %s", 
    					rs.getString(1),
    					rs.getString(2),
    					rs.getString(3)));
    		}
    		System.out.println();
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    public static void queryTwelve(Connection connection) {
    	int dnum, day, month, year;
    	String date;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter Department number: ");
    		dnum = input.nextInt();
    		System.out.print("Enter day completed(DD): ");
    		day = input.nextInt();
    		System.out.print("Enter month completed(MM): ");
    		month = input.nextInt();
    		System.out.print("Enter year completed(YYYY): ");
    		year = input.nextInt();
    		
    		// date
    		date = year + "-" + month + "-" + day;
    		
    		// execute procedure
    		final String jobComplete = "EXEC job_complete '" + dnum + "', '" + date + "'";
    		final ResultSet rs = statement.executeQuery(jobComplete);
    		
    		// print out the job number and types with assembly id
    		System.out.println();
    		while (rs.next()) {
    			System.out.println(String.format("\tJob number: %s | Job Type: %s | Assembly ID: %s", 
    					rs.getString(1),
    					rs.getString(2),
    					rs.getString(3)));
    		}
    		System.out.println();
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }

    public static void queryThirteen(Connection connection) {
    	int low, upp;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter Customer category lower bound(1-10): ");
    		low = input.nextInt();
    		System.out.print("Enter Customer category upper bound(1-10, can be the same): ");
    		upp = input.nextInt();
    		
    		// execute procedure
    		final String getCustomer = "EXEC get_customer '" + low + "', '" + upp + "'";
    		final ResultSet rs = statement.executeQuery(getCustomer);
    		
    		// print out customer name
    		System.out.println();
    		while (rs.next()) {
    			System.out.println(String.format("\tCustomer name: %s | Address: %s | Category: %s", 
    					rs.getString(1),
    					rs.getString(2),
    					rs.getString(3)));
    		}
    		System.out.println();
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    public static void queryFourteen(Connection connection) {
    	int joblb, jobup;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter Job number lower bound: ");
    		joblb = input.nextInt();
    		System.out.print("Enter Job number upper bound: ");
    		jobup = input.nextInt();
    		
    		// execute procedure
    		final String deleteCut = "EXEC delete_cut '" + joblb + "', '" + jobup + "'";
    		final ResultSet rs = statement.executeQuery(deleteCut);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    	
    }
    
    public static void queryFifteen(Connection connection) {
    	int job;
    	String color;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// input prompt
    		System.out.print("Enter Paint Job ID: ");
    		job = input.nextInt();
    		color = input.nextLine();
    		System.out.print("Enter new color: ");
    		color = input.nextLine();
    		
    		// execute procedure
    		final String changeColor = "EXEC change_color '" + job + "', '" + color + "'";
    		final ResultSet rs = statement.executeQuery(changeColor);
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    
    public static void importCustomer(Connection connection) {
    	String fileName, cname, address, skipLine;
    	String category;
    	
    	Scanner input = new Scanner(System.in);
    	
    	try {
    		
    		// prompt file name
    		System.out.print("Enter file name: ");
    		fileName = input.nextLine();
    		
    		// make scanner for it
    		Scanner f = new Scanner(new File(fileName));
    		// delimiter to remove tab commas and spaces (requires file to have spaces)
    		f.useDelimiter(";|\t|\r\n");
    		
    		// while the file has more line, we only get first 3 string between each semi colon
    		while (f.hasNext()) { 
    			// get the first 3 string
      			cname = f.next();
    			address = f.next();
    			category = f.next();
    			
    			try (final Statement statement = connection.createStatement()) {
    				
    				// execute procedure
    				String insertCustomer = "EXEC insert_customer '" + cname + "', '" + address + "', '" + category + "'";
    				ResultSet rs = statement.executeQuery(insertCustomer);
    			
    			} catch (Exception ae) {
    	    		System.out.println(ae);
    	    	}
                
    		}
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
    public static void exportCustomer(Connection connection) {
    	String fileName, cname, address, category;
    	int low, upp;
    	
    	Scanner input = new Scanner(System.in);
    	
    	// ask file name
    	System.out.print("Enter file name: ");
		fileName = input.nextLine();
    	
    	try (final Statement statement = connection.createStatement()) {
    		
    		// make printwriter
    		PrintWriter out = new PrintWriter(fileName, "UTF-8");
    		
    		// prompt lower and upper bount
    		System.out.print("Enter Customer category lower bound(1-10): ");
    		low = input.nextInt();
    		System.out.print("Enter Customer category upper bound(1-10, can be the same): ");
    		upp = input.nextInt();
    		
    		// execute procedure
    		final String getCustomer = "EXEC get_customer '" + low + "', '" + upp + "'";
    		final ResultSet rs = statement.executeQuery(getCustomer);
    		
    		// use result set and write to file
    		while (rs.next()) {
    			out.write(rs.getString(1) + ";" + rs.getString(2) + ";" + rs.getString(3) + "\r\n");
    		}   
    		out.close();
    		
    	} catch (Exception ae) {
    		System.out.println(ae);
    	}
    }
}