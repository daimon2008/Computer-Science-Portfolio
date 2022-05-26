package mod3cs320;

public class Contact {

	private String contactID;
	private String firstName;
	private String lastName;
	private String number;
	private String address;
	
	public Contact(String contactID, String firstName, String lastName, String number, String address) {
		// setting up constructor so if is null or over 10 characters we return that is is invalid
		if (contactID == null || contactID.length()>10 ) {
			throw new IllegalArgumentException("ID is Invalid");
		}
		
		if (firstName == null || firstName.length()>10 ) {
			throw new IllegalArgumentException("First Name is Invalid");
		}
		
		if (lastName == null || lastName.length()>10 ) {
			throw new IllegalArgumentException("Last Name is Invalid");
		}
		
		if (number == null || number.length()>10 ) {
			throw new IllegalArgumentException("Number is Invalid");
		}
		
		if (address== null || address.length()>10 ) {
			throw new IllegalArgumentException("Adress is Invalid");
		}
		//when previous conditions pass we are settings the objects to those entry
		this.setContactID(contactID);
		this.setFirstName(firstName);
		this.setLastName(lastName);
		this.setNumber(number);
		this.setAddress(address);		
	}
	
	public String getContactID() {
		return contactID;
	}
	public void setContactID(String contactID) {
		this.contactID = contactID;
	}
	
	public String getFirstName() {
		return firstName;
	}
	public void setFirstName(String firstName) {
		this.firstName = firstName;
	}
	
	public String getLastName() {
		return lastName;
	}
	public void setLastName(String lastName) {
		this.lastName = lastName;
	}
	
	public String getNumber() {
		return number;
	}
	public void setNumber(String number) {
		this.number = number;
	}
	
	public String getAddress() {
		return address;
	}
	public void setAddress(String address) {
		this.address = address;
	}


		// TODO Auto-generated method stub
		
	
}
