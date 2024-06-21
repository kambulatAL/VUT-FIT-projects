package exceptions;

/**
 * Class that implements custome exception for maze file reading.
 * Game throws this type of error when maze file has invalid number of rows
 * 
 * @author Oleksandr Turytsia (xalaka00)
 * @version 1.0
 */
public class MazeRowOutOfBoundsException extends Exception {

    /**
     * throws exception if length of row from the map file is greater than specified
     */
    public MazeRowOutOfBoundsException() {
        super();
    }

    /**
     * throws exception if length of row from the map file is greater than specified
     * @param message message of the exception
     */
    public MazeRowOutOfBoundsException(String message) {
        super(message);
    }
}
