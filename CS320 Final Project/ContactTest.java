package test;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import mod3cs320.Contact;

class ContactTest {

	@Test
	void testContact() {
		Contact contact = new Contact("1986", "Paul", "Schwartz", "6064712663", "2708 s 9th");
		assertTrue(contact.getContactID().equals("1986"));
		assertTrue(contact.getFirstName().equals("Paul"));
		assertTrue(contact.getLastName().equals("Schwartz"));
		assertTrue(contact.getNumber().equals("6064712663"));
		assertTrue(contact.getAddress().equals("2708 s 9th"));
	}
	// testing individual attributes to long 10>
	@Test
	void ContactIDToLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345678910", "12345", "12345", "12345", "12345");
		});		
	}
	@Test
	void ContactfirstNameToLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", "12345678910", "12345", "12345", "12345");
		});		
	}
	@Test
	void ContactlastNameToLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", "12345", "12345678910", "12345", "12345");
		});		
	}
	@Test
	void ContactNumerToLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", "12345", "12345", "12345678910", "12345");
		});		
	}
	@Test
	void ContactAddressToLong() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", "12345", "12345", "12345", "12345678910");
		});		
	}
//Testing Indivudal atrbutes for null  value	
	@Test
	void ContactIDNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact(null, "12345", "12345", "12345", "12345");
		});		
	}
	
	@Test
	void ContactfirstNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", null, "12345", "12345", "12345");
		});		
	}
	
	@Test
	void ContactlastNameNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", "12345", null, "12345", "12345");
		});		
	}
	
	@Test
	void ContactNumerNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", "12345", "12345", null, "12345");
		});		
	}
	
	@Test
	void ContactAddressNull() {
		Assertions.assertThrows(IllegalArgumentException.class, () -> {
			new Contact("12345", "12345", "12345", "12345", null);
		});		
	}
	

}

	


