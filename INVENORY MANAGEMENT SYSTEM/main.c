#include <stdio.h>
#include <stdlib.h>
#include<ctype.h>
#include<time.h>
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"



struct s_stock {
    int item_id;
    char item_name[50];
    int item_qty;
    float item_price;
};
struct request {
    int item_id;
    int req_qty;
};
void add(struct s_stock item){
    FILE *file = fopen("stock.dat","ab");
    fwrite(&item,sizeof(item),1,file);
    fclose(file);}
void edit(int id, int new_qty,float new_price) {
    struct s_stock item;
    
    FILE *oldqty = fopen("stock.dat", "rb");
    FILE *newqty = fopen("update.dat", "wb");

    if (oldqty == NULL || newqty == NULL) {printf(RED"\n\t\t\tError opening file.\n");return;}
    while (fread(&item, sizeof(item), 1, oldqty)){if (item.item_id == id) {item.item_qty = new_qty;item.item_price=new_price;}
        fwrite(&item, sizeof(item), 1, newqty);
    }
    fclose(oldqty);
    fclose(newqty);
    remove("stock.dat");
    rename("update.dat", "stock.dat");
}
void request_item(int id, int qty) {
    struct request r;

    r.item_id = id;
    r.req_qty = qty;

    FILE *file = fopen("request.dat", "ab");
    if (file == NULL) {
        printf(RED"Error opening request file.\n");
        return;
    }

    fwrite(&r, sizeof(r), 1, file);
    fclose(file);

    printf(GREEN"\n\t\t\t--- Request submitted successfully ---\n");
}
void delete(int id){
    struct s_stock item;
    FILE *oldfile = fopen("stock.dat","rb");
    FILE *newfile = fopen("updated_stock.dat","wb");
    if (oldfile == NULL || newfile == NULL) {
        printf(RED"\n\t\t\tError opening file.\n");
        return;
    }
    while(fread(&item,sizeof(item),1,oldfile)){
        if(item.item_id==id)continue;
    fwrite(&item, sizeof(item), 1, newfile);
    }
    fclose(oldfile);
    fclose(newfile);
    remove("stock.dat");
    rename("updated_stock.dat","stock.dat");
}
void list() {
    struct s_stock item;
    FILE *file = fopen("stock.dat", "rb");
    if (file == NULL) {
        printf(RED "\n\t\t\tError opening file.\n" RESET);
        return;
    }
    printf(CYAN "\n\t\t\t\t--- STOCK LIST ---\n" RESET);
    while (fread(&item, sizeof(item), 1, file)) {
        printf(MAGENTA "\t\t\t ID : %d | Name : %s | Qty : %d |Price : %.2f ",item.item_id, item.item_name, item.item_qty,item.item_price);
        if (item.item_qty < 5) {
            printf(RED "   <-- LOW STOCK!");
        }
        printf("\n" RESET);
    }
    printf(RESET "\t\t\t--------------------------------------\n");
    fclose(file);
}
int add_dupcheck(int id){
    struct s_stock item;
    FILE *file = fopen("stock.dat","rb");
    if(file==NULL)return 0;
    while(fread(&item,sizeof(item),1,file)){
        if(item.item_id== id){fclose(file);return 1;}}
        fclose(file);return 0;}
void sell(int id, int qty) {
    struct s_stock item;
    int found = 0;

    if (qty <= 0) {
        printf(RED "\n\t\t\tInvalid quantity.\n" RESET);
        return;
    }

    FILE *old = fopen("stock.dat", "rb");
    FILE *new = fopen("update.dat", "wb");

    if (!old || !new) {
        printf(RED "\n\t\t\tError opening file.\n" RESET);
        return;
    }

    while (fread(&item, sizeof(item), 1, old)) {

        if (item.item_id == id) {
            found = 1;

            if (item.item_qty < qty) {

                printf(RED "\n\t\t\tNot enough stock available.\n" RESET);

                int choice;
                printf("\t\t\tDo you want to request this item? (1 = Yes, 0 = No): ");
                scanf("%d", &choice);

                if (choice == 1) {
                    request_item(id, qty);
                    
                }

                fclose(old);
                fclose(new);
                remove("update.dat");
                return;
            }

            item.item_qty -= qty;
        }

        fwrite(&item, sizeof(item), 1, new);
    }

    fclose(old);
    fclose(new);

    if (!found) {
        remove("update.dat");
        printf(RED "\n\t\t\tItem ID not found.\n" RESET);
        return;
    }

    remove("stock.dat");
    rename("update.dat", "stock.dat");

    printf(GREEN "\n\t\t\t--- Sale completed ---\n" RESET);
}
void list_requests() {
    struct request r;
    FILE *file = fopen("request.dat", "rb");

    if (file == NULL) {
        printf(MAGENTA"\n\t\t\tNo requests found.\n");
        return;
    }

    printf(CYAN"\n\t\t\t--- REQUEST LIST ---\n");
    while (fread(&r, sizeof(r), 1, file)) {
        printf(YELLOW"\t\t\tItem ID: %d | Requested Qty: %d\n", r.item_id, r.req_qty);
    }
    
    fclose(file);
}
int id_exists(int id) {
    struct s_stock item;
    FILE *f = fopen("stock.dat", "rb");
    if (!f) return 0;
    while (fread(&item, sizeof(item), 1, f)) {
        if (item.item_id == id) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    
    return 0;
}
int clear_request(int id) {
    struct request r;
    struct s_stock item;
    int found = 0;

    FILE *req_old = fopen("request.dat", "rb");
    FILE *req_new = fopen("temp_req.dat", "wb");
    FILE *stock_old = fopen("stock.dat", "rb");
    FILE *stock_new = fopen("update_stock.dat", "wb");

    if (!req_old || !req_new || !stock_old || !stock_new) {
        printf(RED "\n\t\t\tError opening files.\n" RESET);
        return 0;
    }

    int req_qty = 0;

    while (fread(&r, sizeof(r), 1, req_old)) {
        if (r.item_id == id) {
            found = 1;
            req_qty = r.req_qty; 
            continue;  
        }
        fwrite(&r, sizeof(r), 1, req_new);
    }

    fclose(req_old);
    fclose(req_new);

    if (!found) {
        remove("temp_req.dat");
        printf(RED "\n\t\t\tRequest ID not found.\n" RESET);
        return 0;
    }

    while (fread(&item, sizeof(item), 1, stock_old)) {
        if (item.item_id == id) {
            item.item_qty += req_qty;  
        }
        fwrite(&item, sizeof(item), 1, stock_new);
    }

    fclose(stock_old);
    fclose(stock_new);

    remove("request.dat");
    rename("temp_req.dat", "request.dat");

    remove("stock.dat");
    rename("update_stock.dat", "stock.dat");

    printf(GREEN "\n\t\t\tRequest cleared & stock updated.\n" RESET);
    return req_qty;
}
void user_log(const char *action, int item_id, int qty, float price) {
    FILE *log = fopen("log.txt", "a");  
    if (!log) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log,"[DATE: %02d/%02d/%04d ||TIME : %02d:%02d] ACTION: %s | ID: %d | QTY: %d | PRICE: %.2f\n",t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,t->tm_hour, t->tm_min,action, item_id, qty, price);
    fclose(log);
}


int main() {
menu:

    int choice,exit;
    printf(CYAN"\n\t\t\tINVENTORY MANAGEMENT SYSTEM\n");
    printf(BLUE"\n\t\t\t\t---MENU---\n");
    printf(RESET"\t\t\t========================\n");
    printf(CYAN"\t\t\t1. Add Item\n");
    printf(GREEN"\t\t\t2. Sell Item.\n");
    printf(BLUE"\t\t\t3. Update Item.\n");
    printf(MAGENTA"\t\t\t4. Send Request(To Supplier).\n");
    printf(YELLOW"\t\t\t5. List Requests.\n");
    printf(GREEN"\t\t\t6. Clear a Request(By Supplier).\n");
    printf(RED"\t\t\t7. Delete Item.\n");
    printf(YELLOW"\t\t\t8. List stock.\n");
    printf(YELLOW"\t\t\t9. lOG\n");
    printf(RED"\t\t\t10. Exit\n");
    printf(RESET"\t\t\t========================\n");
    printf(GREEN"\t\t\tChoose: ");
    scanf("%d", &choice);
    system("clear");
switch(choice) {

case 1: {
    struct s_stock item;
    int c;

    printf(RESET"\t\t\t========================\n");
    printf("\t\t\tEnter Item ID: ");
    if (scanf("%d", &item.item_id) != 1) {
        printf(RED"\n\t\t\tError: No text allowed.\n");

        while ((c = getchar()) != '\n' && c != EOF);  

        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit == 1) goto menu;
        if(exit==0) goto exit;
    }

    
    if (add_dupcheck(item.item_id)) {
        printf(RED"\t\t\t--- Stock with this ID already exists ---\n");
        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit== 1) goto menu;
        if(exit==0) goto exit;
    }

    printf("\t\t\tEnter Name: ");
    scanf("%s", item.item_name);

    
    for (int i = 0; item.item_name[i] != '\0'; i++)
        item.item_name[i] = toupper(item.item_name[i]);

    printf("\t\t\tEnter Item Quantity: ");
    if (scanf("%d", &item.item_qty) != 1) {
        printf(RED"\n\t\t\tInvalid quantity.\n");

        while ((c = getchar()) != '\n' && c != EOF);

        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit == 1) goto menu;
        if(exit==0)goto exit;
    }

    printf("\t\t\tEnter Item Price: ");
    if (scanf("%f", &item.item_price) != 1) {
        printf(RED"\n\t\t\tInvalid price.\n");

        while ((c = getchar()) != '\n' && c != EOF);

        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit == 1) goto menu;
        if(exit==0) goto exit;
    }

    printf("\t\t\t========================\n");

    add(item);
    user_log("ADD", item.item_id, item.item_qty, item.item_price);

    printf(GREEN"\t\t\t\t\t---Stock created---\n");

    printf(BLUE"\t\t\tPress 1 to goto Menu or Press 0 to Exit: ");
    scanf("%d", &exit);
    system("clear");
    if (exit == 1) goto menu;
    if(exit==0)goto exit;

    break;
}
case 2:{
            struct s_stock item;
        int id,qty,c;
        list();
        printf("\n\t\t\tEnter Item Id :");

if (scanf("%d", &id) != 1) {  
    printf(RED"\n\t\t\tError: No text allowed.\n");

    while ((c = getchar()) != '\n' && c != EOF);  

    printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
    scanf("%d", &exit);
    system("clear");
    if (exit == 1) goto menu;
    if (exit == 0) goto exit;

} else {
    if (!id_exists(id)) {
        printf(RED"\n\t\t\tItem ID not found.\n");
        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit == 1) goto menu;
        if (exit == 0) goto exit;
    }
}

        printf("\n\t\t\tEnter Number of items to sell:");
        scanf("%d",&qty);
       
        sell(id,qty);
        user_log("SELL", id, qty, 0);
             printf(BLUE"\t\t\tPress 1 to goto Menu or Press 0 to Exit:");
               scanf("%d",&exit);
             system("clear");
             if(exit==1)goto menu;
             if(exit==0)goto exit;
        break;
        }    
case 3: {
    int id, new_qty, choice, c;
    float new_price;

    list();

    printf("\n\t\t\tEnter Item ID to edit: ");
    if (scanf("%d", &id) != 1) {  
        printf(RED"\n\t\t\tError: No text allowed.\n");

        while ((c = getchar()) != '\n' && c != EOF);  

        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit == 1) goto menu;
        if (exit == 0) goto exit;
    }

    if (!id_exists(id)) {         
        printf(RED"\n\t\t\tItem ID not found.\n");
        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit == 1) goto menu;
        if (exit == 0) goto exit;
    }

    printf("\n\t\t\tWhat do you want to change?\n");
    printf("\t\t\t1. Quantity only\n");
    printf("\t\t\t2. Price only\n");
    printf("\t\t\t3. Both quantity and price\n");
    printf("\t\t\tChoose: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            printf("\t\t\tEnter new quantity: ");
            scanf("%d", &new_qty);
            {
                struct s_stock temp;
                FILE *file = fopen("stock.dat", "rb");
                while (fread(&temp, sizeof(temp), 1, file)) {
                    if (temp.item_id == id) {
                        new_price = temp.item_price;
                        break;
                    }
                }
                fclose(file);
            }

            edit(id, new_qty, new_price);
           

            break;

        case 2: 
            printf("\t\t\tEnter new price: ");
            scanf("%f", &new_price);
            {
                struct s_stock temp;
                FILE *file = fopen("stock.dat", "rb");
                while (fread(&temp, sizeof(temp), 1, file)) {
                    if (temp.item_id == id) {
                        new_qty = temp.item_qty;
                        break;
                    }
                }
                fclose(file);
            }
            edit(id, new_qty, new_price);
            break;

        case 3: 
            printf("\t\t\tEnter new quantity: ");
            scanf("%d", &new_qty);
            printf("\t\t\tEnter new price: ");
            scanf("%f", &new_price);
            edit(id, new_qty, new_price);
            break;

        default:
            printf("\t\t\tInvalid choice.\n");
            break;
    }
     user_log("EDIT", id, new_qty, new_price);
    system("clear");
    printf(GREEN"\t\t\t--- Item Updated Successfully ---\n"RESET);
    printf(BLUE"\t\t\tPress 1 to goto Menu or 0 to Exit: ");
    scanf("%d", &exit);
    system("clear");
    if (exit == 1) goto menu;
    if (exit == 0) goto exit;
    break;
}
case 4: {
    int id, qty;
    list();
    printf("\n\t\t\tEnter Item ID to request: ");
    scanf("%d", &id);
    printf("\t\t\tEnter quantity to request: ");
    scanf("%d", &qty);
    request_item(id, qty);
    user_log("REQUEST", id, qty, 0);

    printf(BLUE"\n\t\t\tPress 1 to goto Menu or Press 0 to Exit:");
               scanf("%d",&exit);
             system("clear");
             if(exit==1)goto menu;
             if(exit==0)goto exit;
    break;
}
case 5:{
    list_requests();

    printf(BLUE"\n\t\t\tPress 1 to goto Menu or Press 0 to Exit:");
               scanf("%d",&exit);
             system("clear");
             if(exit==1)goto menu;
             if(exit==0)goto exit;
    break;}
case 6: {
    int id,req_qty;
    list_requests();
    printf("\n\t\t\tEnter Item ID to clear : ");
    scanf("%d", &id);
    req_qty=clear_request(id);
    user_log("CLEAR_REQUEST", id, req_qty, 0);

    printf(BLUE "\t\t\tPress 1 to goto Menu or 0 to Exit: ");
    scanf("%d", &exit);
    system("clear");
    if (exit == 1) goto menu;
    if (exit == 0) goto exit;
    break;
}
case 7: {
            int id,c;
             
             list();
            printf("\n\t\t\tEnter Item ID to delete: ");
            if (scanf("%d", &id) != 1) {  
    printf(RED"\n\t\t\tError: No text allowed.\n");

    while ((c = getchar()) != '\n' && c != EOF);  

    printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
    scanf("%d", &exit);
    system("clear");
    if (exit == 1) goto menu;
    if (exit == 0) goto exit;

} else {
    if (!id_exists(id)) {
        printf(RED"\n\t\t\tItem ID not found.\n");
        printf(BLUE"\t\t\tPress 1 to go to Menu or 0 to Exit: ");
        scanf("%d", &exit);
        system("clear");
        if (exit == 1) goto menu;
        if (exit == 0) goto exit;
    }
}
             printf("\n\t\t\t========================\n");
            delete(id);
            user_log("DELETE", id, 0, 0);
             system("clear");
             printf("\t\t\t\t\t---Done---\n");
             printf("\t\t\tPress 1 to goto Menu or Press 0 to Exit:");
               scanf("%d",&exit);
             system("clear");
             if(exit==1)goto menu;
             if(exit==0)goto exit;
            break;
        }
case 8:{
        list();
        printf(BLUE"\n\t\t\tPress 1 to goto Menu or Press 0 to Exit:");
               scanf("%d",&exit);
             system("clear");
             if(exit==1)goto menu;
             if(exit==0)goto exit;
        break;}
case 9: {
    system("clear");
    FILE *log = fopen("log.txt", "r");
    if (!log) {
        printf("\n\t\t\tNo logs found.\n");
        break;
    }

    char line[256];
    printf(CYAN "\n\t\t\t--- LOG HISTORY ---\n");
    while (fgets(line, sizeof(line), log)) {
        printf(YELLOW "\t%s", line);
    }
    fclose(log);
    printf(RESET "\n\t\t\t---------------------------------------------\n");

    printf(BLUE"\n\t\t\tPress 1 for Menu, 0 to Exit: ");
    scanf("%d", &exit);
    system("clear");
    if(exit==1) goto menu;
    if(exit==0) goto exit;
    break;
}
case 10:{
            return 0;}        
default:
printf(RED"\t\t\t\tInvalid choice.\n");
}

goto menu;
exit:
    return 0;
}