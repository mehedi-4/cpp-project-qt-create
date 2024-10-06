#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

class AdminWindow;
class CustomerWindow;

class SuperShopApp : public QWidget {
public:
    SuperShopApp(QWidget *parent = nullptr);

private:
    QPushButton *adminButton;
    QPushButton *customerButton;
    QPushButton *exitButton;
    AdminWindow *adminWindow;
    CustomerWindow *customerWindow;

    void openAdminWindow();
    void openCustomerWindow();
};

class AdminWindow : public QWidget {
public:
    AdminWindow(QWidget *parent = nullptr, QWidget *mainMenu = nullptr);

private:
    QPushButton *addItemButton;
    QPushButton *modifyItemButton;
    QPushButton *removeItemButton;
    QPushButton *showInventoryButton;
    QPushButton *backButton;
    QWidget *mainMenu;

    void addItem();
    void modifyItem();
    void removeItem();
    void showInventory();
    void goBack();
};

class CustomerWindow : public QWidget {
public:
    CustomerWindow(QWidget *parent = nullptr, QWidget *mainMenu = nullptr);

private:
    QPushButton *buyItemButton;
    QPushButton *findByNameButton; // Button to find by name
    QPushButton *showInventoryButton; // Button to show inventory
    QPushButton *backButton;
    QWidget *mainMenu;

    void buyItem();
    void findByName(); // Function to find by name
    void showInventory(); // Function to show inventory
    void generateReceipt(const QString &receiptDetails);
    void goBack();
};


SuperShopApp::SuperShopApp(QWidget *parent) : QWidget(parent) {
    adminButton = new QPushButton("Administration", this);
    customerButton = new QPushButton("Customer", this);
    exitButton = new QPushButton("Exit", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(adminButton);
    layout->addWidget(customerButton);
    layout->addWidget(exitButton);

    connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

    adminWindow = new AdminWindow(nullptr, this);
    customerWindow = new CustomerWindow(nullptr, this);

    connect(adminButton, &QPushButton::clicked, this, &SuperShopApp::openAdminWindow);
    connect(customerButton, &QPushButton::clicked, this, &SuperShopApp::openCustomerWindow);

    setLayout(layout);
}

void SuperShopApp::openAdminWindow() {
    // Ask for username and password
    QString username = QInputDialog::getText(this, "Login", "Enter username:");
    if (username != "admin") {
        QMessageBox::warning(this, "Error", "Invalid username!");
        return;
    }

    QString password = QInputDialog::getText(this, "Login", "Enter password:", QLineEdit::Password); // Password field
    if (password != "admin") {
        QMessageBox::warning(this, "Error", "Invalid password!");
        return;
    }

    // Open the admin window if credentials are correct
    adminWindow->show();
    this->hide();
}


void SuperShopApp::openCustomerWindow() {
    customerWindow->show();
    this->hide();
}

AdminWindow::AdminWindow(QWidget *parent, QWidget *mainMenu) : QWidget(parent), mainMenu(mainMenu) {
    setFixedSize(720, 480);
    addItemButton = new QPushButton("Add Item", this);
    modifyItemButton = new QPushButton("Modify Item", this);
    removeItemButton = new QPushButton("Remove Item", this);
    showInventoryButton = new QPushButton("Show Inventory", this);
    backButton = new QPushButton("Back to Main Menu", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(addItemButton);
    layout->addWidget(modifyItemButton);
    layout->addWidget(removeItemButton);
    layout->addWidget(showInventoryButton);
    layout->addWidget(backButton);

    connect(addItemButton, &QPushButton::clicked, this, &AdminWindow::addItem);
    connect(modifyItemButton, &QPushButton::clicked, this, &AdminWindow::modifyItem);
    connect(removeItemButton, &QPushButton::clicked, this, &AdminWindow::removeItem);
    connect(showInventoryButton, &QPushButton::clicked, this, &AdminWindow::showInventory);
    connect(backButton, &QPushButton::clicked, this, &AdminWindow::goBack);

    setLayout(layout);
}

void AdminWindow::addItem() {
    QString productCode = QInputDialog::getText(this, "Add Item", "Enter product code:");
    if (productCode.isEmpty()) return;

    QString productName = QInputDialog::getText(this, "Add Item", "Enter product name:");
    if (productName.isEmpty()) return;

    int price = QInputDialog::getInt(this, "Add Item", "Enter price:");
    int shelfNumber = QInputDialog::getInt(this, "Add Item", "Enter shelf number:");
    int quantity = QInputDialog::getInt(this, "Add Item", "Enter Quantity:");


    QFile file("inventory.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << productCode << "%20%" << productName << "%20%" << price << "%20%" << shelfNumber << "%20%" << quantity<< "\n";
        file.close();
    }
}

void AdminWindow::modifyItem() {
    QString productCode = QInputDialog::getText(this, "Modify Item", "Enter product code to modify:");
    if (productCode.isEmpty()) return;

    QString newProductName = QInputDialog::getText(this, "Modify Item", "Enter new product name:");
    int newPrice = QInputDialog::getInt(this, "Modify Item", "Enter new price:");
    int newShelfNumber = QInputDialog::getInt(this, "Modify Item", "Enter new shelf number:");
    int newQuantity = QInputDialog::getInt(this, "Modify Item", "Enter new Quantity:");


    QFile file("inventory.txt");
    QFile tempFile("temp.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text) && tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QTextStream out(&tempFile);
        bool found = false;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("%20%");
            if (parts[0] == productCode) {
                out << productCode << "%20%" << newProductName << "%20%" << newPrice << "%20%" << newShelfNumber << "%20%" << newQuantity << "\n";
                found = true;
            } else {
                out << line << "\n";
            }
        }

        file.close();
        tempFile.close();
        file.remove();
        tempFile.rename("inventory.txt");

        if (!found) {
            QMessageBox::information(this, "Modify Item", "Item not found!");
        }
    }
}

void AdminWindow::removeItem() {
    QString productCode = QInputDialog::getText(this, "Remove Item", "Enter product code to remove:");
    if (productCode.isEmpty()) return;

    QFile file("inventory.txt");
    QFile tempFile("temp.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text) && tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QTextStream out(&tempFile);
        bool found = false;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("%20%");
            if (parts[0] == productCode) {
                found = true;
            } else {
                out << line << "\n";
            }
        }

        file.close();
        tempFile.close();
        file.remove();
        tempFile.rename("inventory.txt");

        if (!found) {
            QMessageBox::information(this, "Remove Item", "Item not found!");
        }
    }
}

void AdminWindow::showInventory() {
    QFile file("inventory.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open inventory file.");
        return;
    }

    QString inventoryList;
    QTextStream in(&file);

    QString header = QString("%1 %2 %3 %4 %5\n")
                         .arg("Product Code", 10)
                         .arg("Product Name", 10)
                         .arg("Price", 10)
                         .arg("Shelf No.", 10)
                         .arg("Quantity",10);

    inventoryList += header;
    inventoryList += QString("-").repeated(60) + "\n";

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("%20%");

        if (parts.size() == 5) {
            inventoryList += QString("%1 %2 %3 %4 %5\n")
                .arg(parts[0], 10)
                .arg(parts[1], 10)
                .arg(parts[2], 10)
                .arg(parts[3], 10)
                .arg(parts[4], 10);
        }
    }

    file.close();

    if (inventoryList.isEmpty()) {
        QMessageBox::information(this, "Inventory", "Inventory is empty.");
    } else {
        QMessageBox::information(this, "Inventory", inventoryList);
    }
}

void AdminWindow::goBack() {
    this->hide();
    if (mainMenu) {
        mainMenu->show();
    }
}

CustomerWindow::CustomerWindow(QWidget *parent, QWidget *mainMenu) : QWidget(parent), mainMenu(mainMenu) {
    setFixedSize(720, 480);
    buyItemButton = new QPushButton("Buy Item", this);
    findByNameButton = new QPushButton("Find by Name", this);  // Button for find by name
    showInventoryButton = new QPushButton("Show Inventory", this);  // Button for show inventory
    backButton = new QPushButton("Back to Main Menu", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(buyItemButton);
    layout->addWidget(findByNameButton);  // Add to layout
    layout->addWidget(showInventoryButton);  // Add to layout
    layout->addWidget(backButton);

    connect(buyItemButton, &QPushButton::clicked, this, &CustomerWindow::buyItem);
    connect(findByNameButton, &QPushButton::clicked, this, &CustomerWindow::findByName);  // Connect button to function
    connect(showInventoryButton, &QPushButton::clicked, this, &CustomerWindow::showInventory);  // Connect button to function
    connect(backButton, &QPushButton::clicked, this, &CustomerWindow::goBack);

    setLayout(layout);
}


void CustomerWindow::buyItem() {
    bool continueShopping = true;  // Loop flag
    QString receipt;
    int totalPrice = 0;

    while (continueShopping) {
        QString productCode = QInputDialog::getText(this, "Buy Item", "Enter product code:");
        if (productCode.isEmpty()) return;

        int quantity = QInputDialog::getInt(this, "Buy Item", "Enter quantity:", 1, 1);  // Default quantity is 1

        QFile file("inventory.txt");
        QFile tempFile("temp.txt");

        if (file.open(QIODevice::ReadOnly | QIODevice::Text) && tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QTextStream out(&tempFile);
            bool found = false;
            bool avl = true;
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split("%20%");
                //bool avl = true;
                if (parts[0] == productCode) {
                    int currentQuantity = parts[4].toInt();  // Quantity at index 4
                    int price = parts[2].toInt();  // Extract the price
                    int shelfNumber = parts[3].toInt();  // Shelf number is at index 3
                    //bool avl = true;
                    if (currentQuantity >= quantity) {
                        int remainingQuantity = currentQuantity - quantity;
                        out << parts[0] << "%20%" << parts[1] << "%20%" << price << "%20%" << shelfNumber << "%20%" << remainingQuantity << "\n";

                        receipt += QString("Product: %1 | Price: %2 | Quantity: %3\n")
                                       .arg(parts[1])
                                       .arg(price)
                                       .arg(quantity);
                        totalPrice += price * quantity;
                        found = true;
                    } else {
                        avl = false;
                        QMessageBox::information(this, "Buy Item", "Insufficient quantity available.");
                        out << line << "\n";  // Keep the original line
                    }
                } else {
                    out << line << "\n";  // Keep the original line if not matching
                }
            }

            file.close();
            tempFile.close();
            file.remove();
            tempFile.rename("inventory.txt");

            if (!found && avl) {
                QMessageBox::information(this, "Buy Item", "Item not found!");
                return;
            }
        } else {
            QMessageBox::warning(this, "Error", "Could not open inventory file.");
            return;
        }

        // Ask if the customer wants to buy another item
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Continue Shopping",
                                                                  "Do you want to buy another item?",
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            continueShopping = false;  // Exit loop if customer clicks "No"
        }
    }

    // Generate the final receipt after all purchases
    receipt += QString("\nTotal Price: %1\n").arg(totalPrice);
    generateReceipt(receipt);
}


void CustomerWindow::generateReceipt(const QString &receiptDetails) {
    QMessageBox::information(this, "Receipt", receiptDetails);
}

void CustomerWindow::findByName() {
    QString productName = QInputDialog::getText(this, "Find by Name", "Enter product name:");
    if (productName.isEmpty()) return;

    QFile file("inventory.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open inventory file.");
        return;
    }

    QTextStream in(&file);
    QString productInfo;
    bool found = false;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("%20%");

        if (parts.size() == 5 && parts[1].toLower() == productName.toLower()) {  // Case-insensitive comparison
            productInfo = QString("Product Code: %1\nProduct Name: %2\nPrice: %3\nShelf No.: %4\nQuantity: %5\n")
                              .arg(parts[0])
                              .arg(parts[1])
                              .arg(parts[2])
                              .arg(parts[3])
                              .arg(parts[4]);
            found = true;
            break;
        }
    }

    file.close();

    if (found) {
        QMessageBox::information(this, "Product Information", productInfo);
    } else {
        QMessageBox::information(this, "Product Not Found", "The product with the given name was not found in the inventory.");
    }
}

void CustomerWindow::showInventory() {
    QFile file("inventory.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open inventory file.");
        return;
    }

    QString inventoryList;
    QTextStream in(&file);

    QString header = QString("%1   %2   %3   %4   %5\n")
                         .arg("Product Code", 10)
                         .arg("Product Name", 10)
                         .arg("Price", 10)
                         .arg("Shelf No.", 10)
                         .arg("Quantity", 10);


    inventoryList += header;
    inventoryList += QString("-").repeated(60) + "\n";

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("%20%");

        if (parts.size() == 5) {
            inventoryList += QString("%1   %2   %3   %4   %5\n")
            .arg(parts[0], 10)
                .arg(parts[1], 10)
                .arg(parts[2], 10)
                .arg(parts[3], 10)
                .arg(parts[4], 10);
        }
    }

    file.close();

    if (inventoryList.isEmpty()) {
        QMessageBox::information(this, "Inventory", "Inventory is empty.");
    } else {
        QMessageBox::information(this, "Inventory", inventoryList);
    }
}


void CustomerWindow::goBack() {
    this->hide();
    if (mainMenu) {
        mainMenu->show();
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SuperShopApp window;
    window.resize(720,480);
    window.show();

    return app.exec();
}
