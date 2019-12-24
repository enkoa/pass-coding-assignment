// bao le, austin johnshon, anh vu, ridwan ahmed
// advanced java
// occc spring 2018
// TETRIS BUT WITH RPG ELEMENT AKA GOD MODE
// sceenflip/ loot box

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.Collections;

import java.io.*;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
public class daTetris2 extends JPanel {

	private int wellW = 12, wellH = 24;
	private boolean flip = false;
		
	private final Point[][][] Tetraminos = {
			// I-Piece
			{
				{ new Point(0, 1), new Point(1, 1), new Point(2, 1), new Point(3, 1) },
				{ new Point(1, 0), new Point(1, 1), new Point(1, 2), new Point(1, 3) },
				{ new Point(0, 1), new Point(1, 1), new Point(2, 1), new Point(3, 1) },
				{ new Point(1, 0), new Point(1, 1), new Point(1, 2), new Point(1, 3) }
			},
			
			// J-Piece
			{
				{ new Point(0, 1), new Point(1, 1), new Point(2, 1), new Point(2, 0) },
				{ new Point(1, 0), new Point(1, 1), new Point(1, 2), new Point(2, 2) },
				{ new Point(0, 1), new Point(1, 1), new Point(2, 1), new Point(0, 2) },
				{ new Point(1, 0), new Point(1, 1), new Point(1, 2), new Point(0, 0) }
			},
			
			// L-Piece
			{
				{ new Point(0, 1), new Point(1, 1), new Point(2, 1), new Point(2, 2) },
				{ new Point(1, 0), new Point(1, 1), new Point(1, 2), new Point(0, 2) },
				{ new Point(0, 1), new Point(1, 1), new Point(2, 1), new Point(0, 0) },
				{ new Point(1, 0), new Point(1, 1), new Point(1, 2), new Point(2, 0) }
			},
			
			// O-Piece
			{
				{ new Point(0, 0), new Point(0, 1), new Point(1, 0), new Point(1, 1) },
				{ new Point(0, 0), new Point(0, 1), new Point(1, 0), new Point(1, 1) },
				{ new Point(0, 0), new Point(0, 1), new Point(1, 0), new Point(1, 1) },
				{ new Point(0, 0), new Point(0, 1), new Point(1, 0), new Point(1, 1) }
			},
			
			// S-Piece
			{
				{ new Point(1, 0), new Point(2, 0), new Point(0, 1), new Point(1, 1) },
				{ new Point(0, 0), new Point(0, 1), new Point(1, 1), new Point(1, 2) },
				{ new Point(1, 0), new Point(2, 0), new Point(0, 1), new Point(1, 1) },
				{ new Point(0, 0), new Point(0, 1), new Point(1, 1), new Point(1, 2) }
			},
			
			// T-Piece
			{
				{ new Point(1, 0), new Point(0, 1), new Point(1, 1), new Point(2, 1) },
				{ new Point(1, 0), new Point(0, 1), new Point(1, 1), new Point(1, 2) },
				{ new Point(0, 1), new Point(1, 1), new Point(2, 1), new Point(1, 2) },
				{ new Point(1, 0), new Point(1, 1), new Point(2, 1), new Point(1, 2) }
			},
			
			// Z-Piece
			{
				{ new Point(0, 0), new Point(1, 0), new Point(1, 1), new Point(2, 1) },
				{ new Point(1, 0), new Point(0, 1), new Point(1, 1), new Point(0, 2) },
				{ new Point(0, 0), new Point(1, 0), new Point(1, 1), new Point(2, 1) },
				{ new Point(1, 0), new Point(0, 1), new Point(1, 1), new Point(0, 2) }
			}
	};
	
	private final Color[] tetraminoColors = {
		Color.cyan, Color.blue, Color.orange, Color.yellow, Color.green, Color.pink, Color.red
	};
	
	private Point pieceOrigin;
	private int currentPiece;
	private int rotation;
	private ArrayList<Integer> nextPieces = new ArrayList<Integer>();

	private long score;
	private Color[][] well;

	private int itemPosX, itemPosY;
	private boolean itemExist = false;
	private boolean daFlash = false;
	private boolean diagonal = false;
	private static int theSpeed = 1000;
	private boolean winLose = false, theJoke = false;
	
	// Creates a border around the well and initializes the dropping piece
	private void init() {
		well = new Color[wellW][wellH];
		if ( !flip ) {
			for (int i = 0; i < 12; i++) {
				for (int j = 0; j < 23; j++) {
					if (i == 0 || i == 11 || j == 22) {
						well[i][j] = Color.GRAY;
					} else {
						well[i][j] = Color.BLACK;
					}
				}
			}
		} else {
			for (int i = 0; i < 12; i++) {
				for (int j = 0; j < 23; j++) {
					if (i == 0 || i == 11 || j == 0) {
						well[i][j] = Color.GRAY;
					} else {
						well[i][j] = Color.BLACK;
					}
				}
			}
		}
		newPiece();
	}
	
	// Put a new, random piece into the dropping position
	public void newPiece() {
		if ( !flip ) {
			pieceOrigin = new Point(5, 2);
		} else {
			pieceOrigin = new Point(5, 19);
		}
		rotation = 0;
		if (nextPieces.isEmpty()) {
			Collections.addAll(nextPieces, 0, 1, 2, 3, 4, 5, 6);
			Collections.shuffle(nextPieces);
		}
		currentPiece = nextPieces.get(0);
		nextPieces.remove(0);
	}
	
	// Collision test for the dropping piece as well as items
	private boolean collidesAt(int x, int y, int rotation) {
		if (itemExist == true) {
			for (Point p : Tetraminos[currentPiece][rotation]) { 
				if (p.x + pieceOrigin.x == itemPosX && p.y + pieceOrigin.y == itemPosY) {
					randomizeDrop();
					break;
				}
			}
		}
		for (Point p : Tetraminos[currentPiece][rotation]) { 
			if (well[p.x + x][p.y + y] != Color.BLACK && well[p.x + x][p.y + y] != Color.WHITE ) {
				return true;
			}
		}
		return false;
	}
	
	// Rotate the piece clockwise or counterclockwise
	public void rotate(int i) {
		int newRotation = (rotation + i) % 4;
		if (newRotation < 0) {
			newRotation = 3;
		}
		if (!collidesAt(pieceOrigin.x, pieceOrigin.y, newRotation)) {
			rotation = newRotation;
		}
		repaint();
	}
	
	// Move the piece left or right
	public void move(int i) {
		if (!collidesAt(pieceOrigin.x + i, pieceOrigin.y, rotation)) {
			pieceOrigin.x += i;	
		}
		repaint();
	}
	
	// Drops the piece one line or fixes it to the well if it can't drop
	public void dropDown() {
		if ( !flip ) {
			if (!collidesAt(pieceOrigin.x, pieceOrigin.y + 1, rotation)) {
				pieceOrigin.y += 1;
			} else {
				fixToWell();
			}	
		} else {
			if (!collidesAt(pieceOrigin.x, pieceOrigin.y - 1, rotation)) {
				pieceOrigin.y -= 1;
			} else {
				fixToWell();
			}
		}
		int RNG = (int)(Math.random() * 100);
		if ( RNG == 1 ) {
			flipBoard();
		} else if ( RNG % 10 == 0 && itemExist == false ) {
			spawnItem();
		}
		repaint();
	}

	// Summon blocks that if touches does interesting stuff
	public void spawnItem() {
		do {
			int rngX = (int)(Math.random() * ( 10 - 1 + 1) + 1 );
			int rngY = (int)(Math.random() * ( 19 - 5 + 1) + 5 );
			if ( checkSpace(rngX, rngY) == true ) {
				itemExist = true;
				itemPosX = rngX;
				itemPosY = rngY;
			}
		} while (itemExist == false);
	}

	// Sees if the well spot is empty for an item to spawn
	public boolean checkSpace (int daX, int daY) {
		int c = 0;
		for (int i = daX - 1; i < daX + 2; i++) {
			for (int j = daY - 1; j < daY + 2; j++) {
				if (well[i][j] == Color.BLACK || well[i][j] == Color.GRAY) {
					c ++;
				} 
			}
		}
		if ( c== 9 ) {
			return true;
		} else {
			return false;
		}	
	}
	
	// Determine the random drop when touch then blackout the space
	public void randomizeDrop () 	{
		int theRNG = (int)(Math.random() * 1000);
		if ( theRNG < 200 && theSpeed > 0 ) {
			theSpeed = theSpeed / 2;
		} else if ( theRNG < 400 && theRNG > 200 ) {
			theSpeed = theSpeed + 500;
		} else if ( theRNG < 600 && theRNG > 400 ) {
			theSpeed = 1000 ;
		} else if ( theRNG > 950 && theJoke == false ) {
			theSecret();
			theJoke = true;
		} else if ( theRNG < 700 && theRNG > 600) {
			diagonal = true;
		} else {
			score+= 25;
		}
		if ( !diagonal ) {
			itemExist = false;
		} 	
		well[itemPosX][itemPosY] = Color.BLACK;
	}
	
	// flip the board
	public void flipBoard() {
		Color [][] temp = new Color[12][24];
		for (int i =0 ; i < temp.length; i++) {
			for (int j =0 ; j < temp[0].length; j++) {
				temp[i][j] = well[i][j];
			}
		}
		for (int i = 0; i < 12; i ++) {
			for (int j = 0; j < 23; j++) {
				temp[i][j] = well[i][23 - j - 1];
			}
			
		}
		pieceOrigin.y = 22 - pieceOrigin.y;
		well =  temp;

		if ( itemExist = true ) {
			itemPosY = 22 - itemPosY;
		}

		if ( !flip ) {
			flip = true;
		} else {
			flip = false;
		}

	}
	// Make the dropping piece part of the well, so it is available for
	// collision detection.
	public void fixToWell() {
		
		for (Point p : Tetraminos[currentPiece][rotation]) {
			well[pieceOrigin.x + p.x][pieceOrigin.y + p.y] = tetraminoColors[currentPiece];
		}
		if ( diagonal ) {
			deleteDiagonal();
		} 
		clearRows();
		// delete random leftover whites
		for (int i = 1; i < 12; i ++) {
			for (int j = 0;j < 22; j ++) {
				if (well[i][j] == Color.WHITE) {
					well[i][j] = Color.BLACK;
				}
			}
		}
		if ( !checkLost() ) {
			newPiece();
		} else {
			winLose = true;
			repaint();
		}
	}
	
	public boolean checkLost () {
		if ( !flip ) {
			if (pieceOrigin.y == 2) {
				return(true);
			}
		} else {
			if (pieceOrigin.y == 19) {
				return(true);
			}
		}
		return false;
	}
	//  Delete row
	public void deleteRow(int row) {
		if ( !flip ) {
			for (int j = row-1; j > 0; j--) {
				for (int i = 1; i < 11; i++) {
					well[i][j+1] = well[i][j];
				}
			}
		} else {
			for (int j = row + 1; j < 21; j++) {
				for (int i = 1; i < 11; i++) {
					well[i][j-1] = well[i][j];
				}
			}
		}
	}
	
	// delete diagonally
	public void deleteDiagonal () {
		boolean daLoop = false;
		int x = pieceOrigin.x , y = pieceOrigin.y, c = pieceOrigin.y;
		if ( !flip ) {
			if (pieceOrigin.x <= 5) {
				for (int i = x ; i < 12; i ++) {
					do {
						if (well[i][y + 1] != Color.GRAY) {
							well[i][y + 1] = well[i][y];
						}
						y--;
					} while ( y > 0 );
					if (c < 21) {
						c++;
						y = c;
					} else {
						break;
					}	
				}
			} else {
				for (int i = x ; i > 0; i --) {
					do {
						if (well[i][y + 1] != Color.GRAY) {
							well[i][y + 1] = well[i][y];
						}
						y--;
					} while ( y > 0 );
					if (c < 21) {
						c++;
						y = c;
					} else {
						break;
					}	
				}
			}
		} else {
			if (pieceOrigin.x <= 5) {
				for (int i = x ; i < 12; i ++) {
					do {
						if (well[i][y - 1] != Color.GRAY) {
							well[i][y - 1] = well[i][y];
						}
						y++;
					} while ( y < 21 );
					if (c > 1) {
						c--;
						y = c;
					} else {
						break;
					}	
				}
			} else {
				for (int i = x ; i > 0; i --) {
					do {
						if (well[i][y - 1] != Color.GRAY) {
							well[i][y - 1] = well[i][y];
						}
						y++;
					} while ( y < 21 );
					if (c > 1) {
						c--;
						y = c;
					} else {
						break;
					}	
				}
			}
		}
		repaint();
		diagonal = false;
		itemExist = false;
	}

	// Clear completed rows from the field and award score according to
	// the number of simultaneously cleared rows.
	public void clearRows() {
		boolean gap;
		int numClears = 0;
		
		if ( !flip ) {
			for (int j = 21; j > 0; j--) {
				gap = false;
				for (int i = 1; i < 11; i++) {
					if (well[i][j] == Color.BLACK) {
						gap = true;
						break;
					}
				}
				if (!gap) {
					deleteRow(j);
					j += 1;
					numClears += 1;
				}
			}
		} else {
			for (int j = 1; j < 21; j++) {
				gap = false;
				for (int i = 1; i < 11; i++) {
					if (well[i][j] == Color.BLACK) {
						gap = true;
						break;
					}
				}
				if (!gap) {
					deleteRow(j);
					j -= 1;
					numClears += 1;
				}
			}
		}
		switch (numClears) {
		case 1:
			score += 100;
			break;
		case 2:
			score += 300;
			break;
		case 3:
			score += 500;
			break;
		case 4:
			score += 800;
			break;
		}
	}	
	// Draw the falling piece
	private void drawPiece(Graphics g) {		
		g.setColor(tetraminoColors[currentPiece]);
		for (Point p : Tetraminos[currentPiece][rotation]) {
			g.fillRect((p.x + pieceOrigin.x) * 26, 
					   (p.y + pieceOrigin.y) * 26, 
					   25, 25);
		}
	}
	
	@Override 
	public void paintComponent(Graphics g)
	{
		// Paint the well
		g.fillRect(0, 0, 26*12, 26*23);
		
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 23; j++) {
				g.setColor(well[i][j]);
				g.fillRect(26*i, 26*j, 25, 25);
			}
		}	
		
		// Display the score
		g.setColor(Color.WHITE);
		g.drawString("" + score, 19*12, 25);
		g.drawString("Drop per sec: " + theSpeed/1000.0, 3, 10);
		if ( !diagonal ) {
			g.drawString("Diagonal: No", 3, 22);
		} else {
			g.drawString("Diagonal: Yes", 3, 22);
		}
		if ( winLose ) {
			g.drawString("YOU LOST MATE. GET SHREKT", 70, 300);
		}
		g.drawString("Bao Le    Austin Johnson    Anh Vu    Ridwan Ahamed", 10, 590);
		
		// Draw the currently falling piece
		drawPiece(g);
	}

	public static void main(String[] args) {
		JFrame f = new JFrame("Tetris");
		Image im = Toolkit.getDefaultToolkit().getImage("LOGO.jpg");
   		f.setIconImage(im);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setSize(12*26+10, 26*23+35);
		f.setVisible(true);
		
		final daTetris2 game = new daTetris2();
		game.init();
		f.add(game);
		
		// Keyboard controls
		f.addKeyListener(new KeyListener() {
			public void keyTyped(KeyEvent e) {
			}
			
			public void keyPressed(KeyEvent e) {
				switch (e.getKeyCode()) {
				case KeyEvent.VK_UP:
					game.rotate(-1);
					break;
				case KeyEvent.VK_DOWN:
					game.rotate(+1);
					break;
				case KeyEvent.VK_LEFT:
					game.move(-1);
					break;
				case KeyEvent.VK_RIGHT:
					game.move(+1);
					break;
				case KeyEvent.VK_SPACE:
					game.dropDown();
					game.score += 1;
					break;
				} 
			}
			
			public void keyReleased(KeyEvent e) {
			}
		});
		
		// Make the falling piece drop every second
		new Thread() {
			@Override public void run() {
				while (true) {
					try {
						Thread.sleep(theSpeed);
						game.dropDown();
						game.flashy();
					} catch ( InterruptedException e ) {}
				}
			}
		}.start();
		playSound();
	}
	public void flashy () {
		if ( itemExist == true ) {
			if ( daFlash == true ) {
				well[itemPosX][itemPosY] = Color.BLACK;
				daFlash = false;
			} else {
				well[itemPosX][itemPosY] = Color.WHITE;
				daFlash = true;
			}
		}
	}
	private static Clip dclip;
	public static void playSound() {
		File music = new File("Music.wav");
    		 try {
       			dclip = AudioSystem.getClip();
			dclip.open(AudioSystem.getAudioInputStream(music));
			dclip.start();
			dclip.loop(Clip.LOOP_CONTINUOUSLY);	
    		} catch(Exception ex) {
      			  System.out.println("Error.");
 
 		}
	}
	public void theSecret () {
		dclip.stop();
		File secretFile = new File("SecretM.wav");
		try {
			dclip = AudioSystem.getClip();
			dclip.open(AudioSystem.getAudioInputStream(secretFile));
			dclip.start();
			dclip.loop(Clip.LOOP_CONTINUOUSLY);	
    		} catch(Exception ex) {
      			  System.out.println("Error.");
 
 		}
	}
}