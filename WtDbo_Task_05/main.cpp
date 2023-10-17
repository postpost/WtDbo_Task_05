#include <iostream>
#include <wt/dbo/dbo.h>
#include <wt/Dbo/backend/Postgres.h>
#include "publisher.h"
#include "book.h"
#include "shop.h"
#include "stock.h"
#include "sale.h"

void AddPublisher(Wt::Dbo::Session& session, std::string name) {
	Wt::Dbo::Transaction tr{ session };
	std::unique_ptr<Publisher> publisher(new Publisher{ name });
	Wt::Dbo::ptr<Publisher> publisherPtr = session.add(std::move(publisher));
	tr.commit();
}

void AddBook(Wt::Dbo::Session& session, std::string publisher_name, std::string title) {
	Wt::Dbo::Transaction tr{ session };
	Wt::Dbo::ptr<Publisher>publisher = session.find<Publisher>().where("name=?").bind(publisher_name);
	if (publisher) {
		std::unique_ptr<Book> book(new Book);
		book->title = title;
		publisher.modify()->books.insert(std::move(book));
		tr.commit();
	}
}

void AddShop(Wt::Dbo::Session& session, std::string name) {
	Wt::Dbo::Transaction tr{ session };
	std::unique_ptr<Shop> shops(new Shop{ name });
	Wt::Dbo::ptr<Shop> shopsPtr = session.add(std::move(shops));
	tr.commit();
}

void AddStock(Wt::Dbo::Session& session, std::string shop_name, std::string book_title, int count)
{
	Wt::Dbo::Transaction tr{ session };
	Wt::Dbo::ptr<Shop> shop = session.find<Shop>().where("name=?").bind(shop_name);
	if (shop) {
		std::unique_ptr<Stock> stock(new Stock);
		stock->count = count;
		auto shop_ptr = session.add(std::move(stock));
		shop_ptr.modify()->shop = shop;
		//if possible to make_shared?
		Wt::Dbo::ptr<Book> book = session.find<Book>().where("title=?").bind(book_title);
		std::unique_ptr<Book> b01(new Book{ book_title });
		if (book) {
			//auto book_ptr = session.add(std::move(b01));
			shop_ptr.modify()->book = book;
			//book_ptr.modify()->stock.insert(shop_ptr);
		}
	}
	
	tr.commit();
}

void AddSale(Wt::Dbo::Session& session, std::string date_sale, int price, int count, int stock_id) {
	Wt::Dbo::Transaction tr{ session };
	std::unique_ptr<Sale> sale(new Sale);
	sale->date_sale = date_sale;
	sale->count = count;
	sale->price = price;
	Wt::Dbo::ptr<Stock> stock = session.find<Stock>().where("id=?").bind(stock_id);
	if (stock) {
		//auto stock_ptr = session.add(std::move(sale));
		stock.modify()->sale.insert(std::move(sale));
		if (stock->count > count)
		stock.modify()->count = stock->count - count;
		else {
			std::cout << "Count can not be zero. Total count in stock is: " << stock->count << std::endl;
		}
	}
	else {
		std::cout << "Stock id not found" << std::endl;
	}
	tr.commit();
}

void FindShops(Wt::Dbo::Session& session) {
	Wt::Dbo::Transaction tr01{ session };
	std::cout << "Type publisher name: ";
	std::string p_name = "";
	std::getline(std::cin, p_name);
	//find id by name
	int id = session.query<int>("select id from publisher").where("name=?").bind(p_name);
	//std::cout << "id: " << id << std::endl;
	Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books = session.find<Book>("id").where("publisher_id=?").bind(id);
	std::cout << "You can buy books of the publisher " << p_name
		<< " in the following shops:\n";
	for (auto& book : books) {
		//std::cout << book.id() << std::endl;
		Wt::Dbo::collection<Wt::Dbo::ptr<Stock>>  stocks = session.find<Stock>("shop_id").where("book_id=?").bind(book.id());
		for (auto& stock : stocks) {
			std::cout << stock->shop->name << std::endl;
		}
	}
}

int main() {

	setlocale(LC_ALL, "ru_Ru.UTF-8");
	
	try 
	{
		std::string connStr = "host=localhost "
			"port=5432 "
			"dbname=clientdb "
			"user=postgres "
			"password = 13579";
		
		auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(connStr);
		Wt::Dbo::Session session;
		session.setConnection(std::move(postgres));
		
		session.mapClass<Publisher>("publisher");
		session.mapClass<Book>("book");
		session.mapClass<Shop>("shop");
		session.mapClass<Stock>("stock");
		session.mapClass<Sale>("sale");
		
		try {
			session.createTables();
		}
		catch (std::exception& ex) {
			std::cout << "Tables already exist. " << ex.what() << std::endl;
		}

		//add rows to publisher
		AddPublisher(session, "Samokat");
		AddPublisher(session, "Philosophy");
		AddPublisher(session, "MIF");
		AddPublisher(session, "Alphina book");

		//add rows to book
		AddBook(session, "Samokat", "My day");
		AddBook(session, "Samokat", "Green face");
		AddBook(session, "Philosophy", "Lenin and revolution");
		AddBook(session, "MIF", "How to learn nature");
		AddBook(session, "Alphina book", "Advanced level");

		
		//add rows to Shop
		AddShop(session, "Falanster");
		AddShop(session, "Garage");
		AddShop(session, "Vse svobodny");
		
		//add rows to Stock
		AddStock(session, "Falanster", "Lenin and revolution", 10);
		AddStock(session, "Falanster", "Green face", 10);
		AddStock(session, "Garage", "Green face", 7);
		AddStock(session, "Vse svobodny", "My day", 11);
		AddStock(session, "Vse svobodny", "How to learn nature", 11);


		//add rows to sale
		AddSale(session, "2022-12-01", 1500, 2, 1);
		AddSale(session, "2022-12-02", 500, 1, 2);
		AddSale(session, "2022-12-03", 700, 4, 3);
		

		//информаци€ об издателе (publisher), им€ или идентификатор которого принимаетс€ через std::cin
		//ѕрограмма должна выводить список магазинов, в которых продают книги этого издател€
		//publisher_id => book_id => stock => shop_id
		FindShops(session);
	}

	catch (std::exception& ex) {
		std::cout << "Connection was not performed.\t" << ex.what() << std::endl;
	}
	return 0;

}

