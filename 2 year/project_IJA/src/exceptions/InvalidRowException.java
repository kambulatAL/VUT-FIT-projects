package exceptions;

/**
 * Class that implements custome exception for maze file reading.
 * Game throws this type of error when maze file has invalid number of rows (it
 * differs from specification)
 * 
 * @author Oleksandr Turytsia (xalaka00)
 * @version 1.0
 */
public class InvalidRowException extends Exception {
    /**
     * Exception is thrown when an invalid row from the map file was found
     */
    public InvalidRowException() {
        super();
    }

    /**
     * Exception is thrown when an invalid row from the map file was found
     * @param message message of the exception
     */
    public InvalidRowException(String message) {
        super(message);
    }
}
