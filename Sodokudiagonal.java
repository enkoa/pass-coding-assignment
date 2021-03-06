// BAO LE
// OCCC 2018
// advanced java
// 9x9 sodoku with diagonal

import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;
public class Sodokudiagonal {
	static final int BLANK = -1;
	static int MIN_VALUE = 1, MAX_VALUE = 9;
	public static void main(String[] args) {
		String fileName;
		Scanner input = new Scanner(System.in);
		if (args.length == 1) {
			fileName = args[0];
		} else {
			System.out.print("Enter a file for sodoku: ");
			fileName = input.nextLine();
			input.close();
		}
		
		try {
			Scanner f = new Scanner(new File(fileName));
			int thePuzzle[][] = new int[9][9];
			char ch;
			for(int i = 0; i < thePuzzle.length;i++) {
				for(int j = 0; j < thePuzzle[i].length; j++) {
					ch = f.next().charAt(0);
					if (ch < '1' || ch > '9') {
						thePuzzle[i][j] = BLANK;
					} else {
						thePuzzle[i][j] = (int)(ch - 48);
					}

				}
			}
			System.out.println("Before: ");
			displayPuzzle(thePuzzle);
			System.out.println();
			if (solve(thePuzzle)) {
				System.out.println("Solved: ");
				displayPuzzle(thePuzzle);
			} else {
				System.out.println("Invalid Puzzle.");
			}
		
		} catch (FileNotFoundException fnfe) {
			System.out.println("File not found");
			System.out.println(fnfe.toString());
			System.exit(1);
		}
		
	}

	public static boolean solve(int thePuzzle[][]) {
		if (isComplete(thePuzzle) ) {
			return isValid(thePuzzle);
		} 
		if (!isValid(thePuzzle)) {
			return(false);
		}
		// find first blank space, at row,col
		
		int row = 0,col = 0;
		for(int r = 0; r < thePuzzle.length;r++) {
			for(int c = 0; c < thePuzzle[r].length;c++) {
				if (thePuzzle[r][c] == BLANK) {
					row = r;
					col = c;
				}
			}
		}
		boolean result;
		for (int x = MIN_VALUE; x <= MAX_VALUE; x++) {
			thePuzzle[row][col] = x;
			result = solve(thePuzzle);
			if (result) {
				return(true);
			}
		}
		thePuzzle[row][col] = BLANK;
		return(false);		
	}
	public static boolean isComplete(int thePuzzle[][]) {
		boolean result = true;
		for(int r = 0; r < thePuzzle.length;r ++) {
			for (int c = 0; c < thePuzzle[0].length; c++) {
				if (thePuzzle[r][c] == -1) {
					result = false;
				}
			}
		}
		return(result);
	}
	public static boolean isValid(int thePuzzle[][]) {
		return(rowCheck(thePuzzle) && columnCheck(thePuzzle) && gridCheck(thePuzzle) && diagonalCheck(thePuzzle));
	}
	public static boolean diagonalCheck(int thePuzzle[][]) {
		boolean[] check = new boolean[thePuzzle.length + 1];
		int value;
		// check top left to bottom right diagonal
		for (int r = 0; r < thePuzzle.length; r++) {
			value = thePuzzle[r][r];
			if (value == -1) {
				
			} else if (!check[value]) {
				check[value] = true;
			} else {
				return(false);
			}
		}
		// set the check to false again
		for (int i =0; i < check.length; i ++) {
			check[i] = false;
		}
		// check the top right to bottom left
		for (int r = 0; r < thePuzzle.length; r++) {
			value = thePuzzle[r][thePuzzle.length - r - 1];
			if (value == -1) {
				
			} else if (!check[value]) {
				check[value] = true;
			} else {
				return(false);
			}
		}

		return(true);
		
	}
	public static boolean rowCheck(int thePuzzle[][]) {
		boolean[] check = new boolean[thePuzzle.length + 1];
		int value;
		for (int r = 0; r < thePuzzle.length; r++) {
			for(int c = 0; c < thePuzzle.length; c++) {
				value = thePuzzle[r][c];
				if (value == -1) {

				} else if (!check[value]) {
					check[value] = true;
				} else {
					return(false);
				}
			}
			for (int i = 0; i < check.length; i ++) {
				check[i] = false;
			}
		}
		return(true);
	}
	public static boolean columnCheck(int thePuzzle[][]) {
		boolean[] check = new boolean[thePuzzle.length + 1];
		int value;
		for (int r = 0; r < thePuzzle.length; r++) {
			for(int c = 0; c < thePuzzle.length; c++) {
				value = thePuzzle[c][r];
				if (value == -1) {

				} else if (!check[value]) {
					check[value] = true;
				} else {
					return(false);
				}
			}
			for (int i = 0; i < check.length; i ++) {
				check[i] = false;
			}
		}
		return(true);
	}
	public static boolean gridCheck(int thePuzzle[][]) {
		int subSize = (int) (Math.sqrt(thePuzzle.length));
		for (int r= 0; r < thePuzzle.length; r+= subSize) {
			for (int c = 0; c < thePuzzle.length; c += subSize) {
				if (!checkSquare(thePuzzle, r, c, subSize)) {
					return(false);
				}
			}
		}
		return(true);		
	}
	public static boolean checkSquare(int thePuzzle[][], int row, int column, int subSize) {
		boolean[] check = new boolean[thePuzzle.length +1];
		int value;
		for (int r = row; r < (row + subSize); r++) {
			for (int c = column; c < (column + subSize); c++) {
				value = thePuzzle[r][c];
				if (value == -1) {
					
				} else if (!check[value]) {
					check[value] = true;	
				} else {
					return(false);
				}
			}
		}
		return(true);
	}
	public static void displayPuzzle (int thePuzzle[][]) {
		for (int i = 0; i < thePuzzle.length; i ++) {
			if (i % 3 == 0) {
				System.out.println();
			}
			for(int j = 0; j < thePuzzle[0].length; j ++) {
				if (j % 3 == 0) {
					System.out.print("\t");
				}
				if (thePuzzle[i][j] == -1) {
					System.out.printf("%3s", "-");
				} else {
					System.out.printf("%3d", thePuzzle[i][j]);
				}
			}
			System.out.println();
		}
	}
}