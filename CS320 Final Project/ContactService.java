package mod3cs320;

import java.util.ArrayList;

public class ContactService {
//Setting up array list
		private ArrayList<Contact> contacts;
		//Setting up constructor
		 public ContactService() {
			 contacts = new ArrayList<>();
		 }
		 
		 //Setting up being able to add contact to the list, if not already existing
		 public boolean addContact(Contact contact) {
			 //seeing weather contact exists or not			 
			 if(!contacts.contains(contact)) {
				 return contacts.add(contact);
			 }
		    return false;
		 }
		 //Setting up for removing a contact by contactID
		 public boolean deleteContact(String contactID)  {
			 for(Contact contactElement : contacts) {
				 if(contactElement.getContactID().equals(contactID))  {
					 return contacts.remove(contactElement);
				 }
			 }
			 return false;
			 		 }
		 
		 //Setting up to be able to update infromation within contacts, ID, FN, LN, Num, Addr, akign sure not more than 10 as well
		 public boolean updateContact(String contactID, String firstName, String lastName, String number, String address) {
			 for(Contact contactList : contacts) {
				 if(contactList.getContactID().equals(contactID)) {
					 if(!firstName.equals("") && !(firstName.length()>10))
						 contactList.setFirstName(firstName);
					 if(!lastName.equals("") && !(lastName.length()>10))
						 contactList.setLastName(lastName);
					 if(!number.equals("") && !(number.length()>10))
						 contactList.setNumber(number);
					 if(!address.equals("") && !(address.length()>10))
						 contactList.setAddress(address);				 
					 return true;
				 }
			 }
			 return false;
		 }
		
}
			 


