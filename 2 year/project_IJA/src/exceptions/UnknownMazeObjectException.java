package exceptions;

/**
 * Class that implements custome exception for maze file reading.
 * Game throws this type of error when maze file has invalid maze object
 * 
 * @author Oleksandr Turytsia (xalaka00)
 * @version 1.0
 */
public class UnknownMazeObjectException extends Exception {
    /**
     * Exception which is thrown when undefined maze object was found
     */
    public UnknownMazeObjectException() {
        super();
    }

    /**
     * Exception which is thrown when undefined maze object was found
     * @param message message of the exception
     */
    public UnknownMazeObjectException(String message) {
        super(message);
    }
}
