package game.objects;

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Image;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

import exceptions.MazeRowOutOfBoundsException;
import exceptions.UnknownMazeObjectException;
import game.common.CommonField;
import game.common.CommonMaze;
import game.common.CommonField.Direction;
import gui.components.Label;

/**
 * Class representing maze object itself.
 * It contains methods to work with GUI and abstract part of the maze.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class Maze implements CommonMaze {

    private int id;
    private int cols;
    private int rows;
    private boolean isPause = true;
    private boolean isReview;
    private PacmanObject pacman;
    private KeyObject key;

    private final List<GhostObject> ghosts = new ArrayList<>();
    private final List<List<CommonField>> fields = new ArrayList<>();

    private final MazeComponent mazeComponent = new MazeComponent();

    private File mazeFile;

    /**
     * Contructor of the maze object
     * @param cols number of columns in the maze
     * @param rows number of rows in the maze
     * @param mazeFile file with the maze
     */
    public Maze(int cols, int rows, File mazeFile) {
        this.cols = cols;
        this.rows = rows;
        this.mazeFile = mazeFile;
    }

    /**
     * Contructor of the maze object
     * @param cols number of columns in the maze
     * @param rows number of rows in the maze
     * @param mazeFile file with the maze
     * @param isReview specifies review mode
     */
    public Maze(int cols, int rows, File mazeFile, boolean isReview) {
        this(cols, rows, mazeFile);
        this.isReview = isReview;
    }

    /**
     * Returns whether the maze is in state of review,
     * which indicates that maze is being used to watch game not to play it.
     * 
     * @return true if maze is in state of review
     */
    public boolean getReview() {
        return this.isReview;
    }

    /**
     * Returns pause state. In pause state player unable to control anything
     * 
     * @return true if the maze is in pause state
     */
    public boolean getPause() {
        return isPause;
    }

    /**
     * Pause handler
     * 
     * @param pause boolean value if pause is enabled
     */
    public void setPause(boolean pause) {
        isPause = pause;
    }

    /**
     * Returns part of the maze which is responsible for GUI
     * 
     * @return object of MazeComponent
     */
    public MazeComponent getMazeComponent() {
        return this.mazeComponent;
    }

    /**
     * Returns name of file without extension
     * that was used to construct maze
     * 
     * @return returns name of the maze
     */
    public String getMazeName() {
        Pattern pattern = Pattern.compile("(.*)\\.");
        Matcher matcher = pattern.matcher(mazeFile.getName());

        if (matcher.find()) {
            return matcher.group(1);
        }
        return "Maze";
    }

    /**
     * Returns file that was used in order to
     * construct maze itself
     * 
     * @return maze's file
     */
    public File getMazeFile() {
        return this.mazeFile;
    }

    /**
     * Creates ghost and puts it at given row & column
     * 
     * @param x column index
     * @param y row index
     * @return Returns newly created field object
     */
    private PathField createGhost(int x, int y) {
        PathField field = new PathField(this, x, y);
        GhostObject ghost = new GhostObject(field);
        ghosts.add(ghost);
        field.put(ghost);

        return field;
    }

    /**
     * Creates pathfield at given row & column
     * 
     * @param x column index
     * @param y row index
     * @return Returns newly created pathfield object
     */
    private PathField createKey(int x, int y) {
        PathField field = new PathField(this, x, y);
        this.key = new KeyObject(field);
        field.put(this.key);

        return field;
    }

    /**
     * Creates finish object and puts it at given row & column
     * 
     * @param x column index
     * @param y row index
     * @return Returns newly created field object
     */
    private PathField createFinish(int x, int y) {
        PathField field = new PathField(this, x, y);
        field.put(new FinishObject(field));

        return field;
    }

    /**
     * Creates pacman object and puts it at given row & column
     * 
     * @param x column index
     * @param y row index
     * @return Returns newly created field object
     */
    private PathField createPacman(int x, int y) {
        PathField field = new PathField(this, x, y);
        pacman = new PacmanObject(field);
        field.put(pacman);

        mazeComponent.createHealthContainer();

        return field;
    }

    /**
     * Creates point object and puts it at given row & column
     * 
     * @param x column index
     * @param y row index
     * @return Returns newly created field object
     */
    private PathField createPoint(int x, int y) {
        PathField field = new PathField(this, x, y);
        field.put(new PointObject(field));

        return field;
    }

    /**
     * Returns pacman (player)
     * 
     * @return pacman
     */
    public PacmanObject getPacman() {
        return this.pacman;
    }

    @Override
    public void parseLine(String line) {
        ArrayList<CommonField> row = new ArrayList<>();
        try {
            if (fields.size() >= rows) {
                throw new MazeRowOutOfBoundsException("Too much rows in a file");
            }

            for (char c : line.toCharArray()) {
                int y = fields.size();
                int x = row.size();
                switch (c) {
                    case 'X':
                        row.add(new WallField(this, x, y));
                        break;
                    case 'G':
                        row.add(createGhost(x, y));
                        break;
                    case 'K':
                        row.add(createKey(x, y));
                        break;
                    case 'T':
                        row.add(createFinish(x, y));
                        break;
                    case 'S':
                        row.add(createPacman(x, y));
                        break;
                    case '.':
                        row.add(createPoint(x, y));
                        break;
                    default:
                        throw new UnknownMazeObjectException("Object " + c + " is not valid");
                }
            }

            fields.add(row);
        } catch (MazeRowOutOfBoundsException | UnknownMazeObjectException e) {
            e.printStackTrace();
        }

    }

    /**
     * Check for key existence in the maze.
     * 
     * @return true if the key exists
     */
    public boolean hasKey() {
        return this.key != null;
    }

    @Override
    public CommonField getField(int x, int y) {
        try {
            return fields.get(y).get(x);
        } catch (IndexOutOfBoundsException e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    public int numRows() {
        return this.rows;
    }

    @Override
    public int numCols() {
        return this.cols;
    }

    @Override
    public List<GhostObject> ghosts() {
        return this.ghosts;
    }

    @Override
    public int nextId() {
        return id++;
    }

    /**
     * Inverts given direction. It is used in order
     * to roll back packman and ghost movement
     * 
     * @param dir direction that needs to be inverted
     * @return Returns inverted direction
     */
    public Direction invertDirection(Direction dir) {
        switch (dir) {
            case L:
                return Direction.R;
            case U:
                return Direction.D;
            case D:
                return Direction.U;
            case R:
                return Direction.L;
        }
        return null;
    }

    /**
     * The maze can be controled using special instructions. This function
     * takes instruction string in, parses and interprets it.
     * 
     * @param instruction given instruction
     * @param isInverted  applies 'backward' instruction if true
     */
    public void processInstruction(String instruction, boolean isInverted) {
        Pattern pattern = Pattern.compile("^(GHOST|PACMAN)(\\d*)?\\s(R|U|L|D)$");

        Matcher matcher = pattern.matcher(instruction);

        matcher.find();

        String entity = matcher.group(1);
        int id = Integer.valueOf(matcher.group(2).length() > 0 ? matcher.group(2) : "-1");
        Direction dir = CommonField.Direction.valueOf(matcher.group(3));

        if (isInverted) {
            dir = invertDirection(dir);
        }

        if (entity.equals("PACMAN")) {
            if (isInverted) {
                pacman.undoScore();
                pacman.undoKey();
            }
            pacman.move(dir, isInverted);
        } else if (entity.equals("GHOST")) {
            for (GhostObject ghost : ghosts) {
                if (ghost.getId() == id) {
                    ghost.move(dir, false);
                }
            }
        }
    }

    /**
     * Nested class that works specifically with GUI.
     * It creates special Labels and panels that are being updated right away and
     * can
     * be used at user interface.
     * 
     * @author Oleksandr Turytsia (xturyt00)
     * @version 1.0
     */
    public class MazeComponent {
        private final Label scoreText = new Label("Score: 0");
        private final JPanel healthContainer = new JPanel(new GridLayout(1, 3));
        private final JLabel keyContainer = new JLabel();

        /**
         * Constructor of the MazeComponent
         */
        public MazeComponent() {
            healthContainer.setOpaque(false);
            keyContainer.setHorizontalAlignment(SwingConstants.CENTER);
            healthContainer.setPreferredSize(new Dimension(200, 20));
        }

        /**
         * Updates score label
         */
        public void updateScore() {
            scoreText.setText("Score: " + getPacman().getScore());
        }

        /**
         * Updated key label
         */
        public void updateKey() {

            if (pacman.getKey() == null) {
                keyContainer.setIcon(null);
            } else {
                Image icon = new ImageIcon("lib/sprites/game/key.png").getImage();
                keyContainer.setIcon(new ImageIcon(icon.getScaledInstance(40, 40, Image.SCALE_SMOOTH)));
            }
        }

        /**
         * Updates health container
         */
        public void updateHealth() {
            Image heartImage = new ImageIcon("lib/sprites/game/heart.png").getImage();
            JLabel heart = new JLabel(new ImageIcon(heartImage.getScaledInstance(40, 40, Image.SCALE_SMOOTH)));

            for (int i = healthContainer.getComponentCount(); i > getPacman().getLives() && i > 0; i--) {
                healthContainer.remove(i - 1);
            }

            for (int i = healthContainer.getComponentCount(); i < getPacman().getLives(); i++) {
                healthContainer.add(heart);
            }
        }

        /**
         * Creates health container
         */
        private void createHealthContainer() {
            for (int i = 0; i < getPacman().getLives(); i++) {
                Image heartImage = new ImageIcon("lib/sprites/game/heart.png").getImage();
                JLabel heart = new JLabel(new ImageIcon(heartImage.getScaledInstance(40, 40, Image.SCALE_SMOOTH)));
                healthContainer.add(heart);
            }
        }

        /**
         * Getter for score label
         * 
         * @return score label
         */
        public Label getScoreText() {
            return this.scoreText;
        }

        /**
         * Getter for health container
         * 
         * @return health container
         */
        public JPanel getHealthContainer() {
            return this.healthContainer;
        }

        /**
         * Getter for key label
         * 
         * @return key label
         */
        public JLabel getKeyContainer() {
            return this.keyContainer;
        }
    }

}
