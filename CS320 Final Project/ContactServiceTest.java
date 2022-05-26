package test;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import mod3cs320.Contact;
import mod3cs320.ContactService;

class ContactServiceTest {
//adding a contact test
	@Test
	void testAddContact() {
		ContactService cs = new ContactService();
		Contact contact = new Contact("1986", "Paul", "Schwartz", "6064712663", "2708 s 9th");
		assertTrue(cs.addContact(contact));
		
		contact = new Contact("1986", "Paul", "Schwartz", "6064712663", "2708 s 9th");
		assertEquals(true, cs.addContact(contact));
	}
	//adding a contact test failure, to many didigits
	@Test
	void testAddContactFail() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {		
		new Contact ("19861111111111", "Paul", "Schwartz", "6064712663", "2708 s 9th");
		});
	}
	//testing deleting a contact
	@Test
	void testDeleteContact() {
		ContactService cs = new ContactService();
		Contact contact = new Contact("1986", "Paul", "Schwartz", "6064712663", "2708 s 9th");
		cs.addContact(contact);
		//true if we are able to delete contacnt with contactID 1986
		assertTrue(cs.deleteContact("1986"));
	}
	//creating test where deleting contact fails
	@Test
	void testDeleteContactFail() {
		ContactService cs = new ContactService();
		Contact contact = new Contact("1986", "Paul", "Schwartz", "6064712663", "2708 s 9th");
		cs.addContact(contact);
		//we try to delete a contact with contractID 1986, does not exist so it returns false
		assertFalse(cs.deleteContact("1987"));
	}
	//testing updating contact
	@Test
	void testUpdateContact() {
		ContactService cs = new ContactService();
		Contact contact = new Contact("1986", "Paul", "Schwartz", "6064712663", "2708 s 9th");
		cs.addContact(contact);
		assertTrue(cs.updateContact("1986", "john", "Boo", "2021", "face st"));
	}
	//update contact test fail
	@Test
	void testUpdateContactFail() {
			ContactService cs = new ContactService();
			Contact contact = new Contact("1986", "Paul", "Schwartz", "6064712663", "2708 s 9th");
			cs.addContact(contact);
			//can not update with null values, so test passes as is is false
			assertFalse(cs.updateContact(null, null, null, null, null));	
}
}




