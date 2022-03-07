#include "a1.h"

/**
	Add your functions to this file.
	Make sure to review a1.h.
	Do NOT include a main() function in this file
	when you submit.
*/
Menu *load_menu(char *fname)
{
	int number_of_items = 0;
	char line[255];

	Menu *menu = malloc(sizeof(Menu));

	FILE *fp = fopen(fname, "r");

	while (fgets(line, 255, fp) != NULL)
	{
		number_of_items++;
	}

	fclose(fp);

	// memory allocation
	menu->num_items = number_of_items;
	menu->item_codes = malloc(sizeof(char *) * menu->num_items); // each array slot is a char pointer (string pointer)
	menu->item_names = malloc(sizeof(char *) * menu->num_items);
	menu->item_cost_per_unit = malloc(sizeof(double) * menu->num_items);

	FILE *fp2 = fopen(fname, "r");

	int count = 0;
	while (fgets(line, 255, fp2) != NULL)
	{
		char *temp_string;
		(menu->item_codes)[count] = malloc(sizeof(char) * ITEM_CODE_LENGTH);
		strcpy((menu->item_codes)[count], strtok(line, MENU_DELIM));
		// (menu->item_codes)[count] = temp_string;

		(menu->item_names)[count] = malloc(sizeof(char) * MAX_ITEM_NAME_LENGTH);
		strcpy((menu->item_names)[count], strtok(NULL, MENU_DELIM));

		char *remaining;
		double result;
		char *temp = strtok(NULL, "$");
		result = strtod(temp, &remaining);

		(menu->item_cost_per_unit)[count] = result;

		count++;
	}

	fclose(fp2);

	return menu;
}

Restaurant *initialize_restaurant(char *name)
{
	Restaurant *restaurant = malloc(sizeof(Restaurant));
	restaurant->name = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(restaurant->name, name);
	// restaurant->menu = malloc(sizeof(Menu));
	// restaurant->pending_orders = malloc(sizeof(Queue));

	// intitialize to contents of menu_fname
	restaurant->num_completed_orders = 0;
	restaurant->num_pending_orders = 0;

	// pointng to an empty queue
	Queue *queue = malloc(sizeof(Queue));
	queue->head = NULL;
	queue->tail = NULL;

	restaurant->pending_orders = queue;

	restaurant->menu = load_menu(MENU_FNAME);

	return restaurant; // returning pointer to Restaruant.
}

Order *build_order(char *items, char *quantities)
{
	char quant[MAX_ITEM_QUANTITY_DIGITS];
	strcpy(quant, quantities);

	Order *order = malloc(sizeof(Order));
	order->num_items = strlen(items) / 2;

	// changing quantities to an int array

	order->item_quantities = malloc(sizeof(int) * order->num_items);

	char *section;
	section = strtok(quant, MENU_DELIM);

	(order->item_quantities)[0] = atoi(section);

	for (int i = 1; i < (order->num_items); i++)
	{
		section = strtok(NULL, MENU_DELIM);
		(order->item_quantities)[i] = atoi(section);
	}

	// changing items into array of char pointers set to item codes in order
	order->item_codes = malloc(sizeof(char *) * order->num_items);

	int count = 0;
	char temp[ITEM_CODE_LENGTH];
	char temp2[ITEM_CODE_LENGTH - 1];
	char items_temp[MAX_ITEM_NAME_LENGTH];

	for (int i = 0; i < (strlen(items) - 1); i = i + 2)
	{
		(order->item_codes)[count] = malloc(sizeof(ITEM_CODE_LENGTH));
		temp[1] = '\0';
		temp2[1] = '\0';
		temp[0] = items[i];
		temp2[0] = items[i + 1];

		strcat(temp, temp2);
		strcpy((order->item_codes)[count], temp);
		count++;
	}
	// printf("%s\n", order->item_codes[2]);
	// works

	// Free all memory.

	return order;
}

// Managing the order queue
void enqueue_order(Order *order, Restaurant *restaurant)
{
	// FIFO, first in first out (Queue)
	QueueNode *q = malloc(sizeof(QueueNode));
	q->order = order;
	q->next = NULL;
	restaurant->num_pending_orders += 1;

	if ((restaurant->pending_orders)->tail == NULL)
	{
		// empty queue, make the order the head and tail.
		(restaurant->pending_orders)->tail = q;
		(restaurant->pending_orders)->head = q;
		return;
	}

	// current tail points to new added node
	restaurant->pending_orders->tail->next = q;
	// new added node is now the new tail.
	restaurant->pending_orders->tail = q;
}

Order *dequeue_order(Restaurant *restaurant)
{
	// Assume the Queue is neveer empty when dequeing

	QueueNode *removed = restaurant->pending_orders->head;
	Order *order_dequeued = removed->order;
	restaurant->pending_orders->head = restaurant->pending_orders->head->next;

	free(removed);

	if (restaurant->pending_orders->head == NULL)
	{
		restaurant->pending_orders->tail = NULL;
	}

	restaurant->num_completed_orders += 1;
	restaurant->num_pending_orders -= 1;

	return order_dequeued;
}

/*
	Getting information about our orders and order status
*/

double get_item_cost(char *item_code, Menu *menu)
{
	// ASSMPTION: the code is in the menu.
	int temp = 0;
	for (int i = 0; i < menu->num_items; i++)
	{
		if (strcmp((menu->item_codes)[i], item_code) == 0)
		{
			temp = i;
		}
	}

	return (menu->item_cost_per_unit)[temp];
}

double get_order_subtotal(Order *order, Menu *menu)
{
	double total = 0;
	for (int i = 0; i < order->num_items; i++)
	{
		total += (order->item_quantities)[i] * get_item_cost((order->item_codes)[i], menu);
	}
	return total;
}

double get_order_total(Order *order, Menu *menu)
{
	double total = 0;
	for (int i = 0; i < order->num_items; i++)
	{
		total += (order->item_quantities)[i] * get_item_cost((order->item_codes)[i], menu);
	}

	total = total * (double)(1 + ((double)TAX_RATE / (double)100));
	return total;
}

int get_num_completed_orders(Restaurant *restaurant)
{
	int temp = restaurant->num_completed_orders;
	return temp;
}

int get_num_pending_orders(Restaurant *restaurant)
{
	int temp = restaurant->num_pending_orders;
	return temp;
}

/*
	Closing down and deallocating memory
*/
void clear_order(Order **order)
{
	for (int i = 0; i < (*order)->num_items; i++)
	{
		free(((*order)->item_codes)[i]);
	}

	free((*order)->item_codes);
	free((*order)->item_quantities);
	free(*order);
	*order = NULL;
}

void clear_menu(Menu **menu)
{
	free((*menu)->item_cost_per_unit);

	for (int i = 0; i < (*menu)->num_items; i++)
	{
		free(((*menu)->item_codes)[i]);
		free(((*menu)->item_names)[i]);
	}
	free((*menu)->item_names);
	free((*menu)->item_codes);
	free(*menu);
	*menu = NULL;
}

void close_restaurant(Restaurant **restaurant)
{

	// freeing the queue
	QueueNode *temp;
	// Order *dequeue = malloc(sizeof(Order));

	if ((*restaurant)->pending_orders->head != NULL)
	{
		while ((*restaurant)->pending_orders->head->next != NULL)

		{
			temp = (*restaurant)->pending_orders->head;

			(*restaurant)->pending_orders->head = (*restaurant)->pending_orders->head->next;

			clear_order(&(temp->order)); // clearing the actual order for each queue node
			free(temp);
		}
	}

	free((*restaurant)->pending_orders->head);
	free((*restaurant)->pending_orders);
	free((*restaurant)->name);
	clear_menu(&((*restaurant)->menu));
	free(*restaurant);

	*restaurant = NULL;
}

void print_menu(Menu *menu)
{
	fprintf(stdout, "--- Menu ---\n");
	for (int i = 0; i < menu->num_items; i++)
	{
		fprintf(stdout, "(%s) %s: %.2f\n",
				menu->item_codes[i],
				menu->item_names[i],
				menu->item_cost_per_unit[i]);
	}
}

void print_order(Order *order)
{
	for (int i = 0; i < order->num_items; i++)
	{
		fprintf(
			stdout,
			"%d x (%s)\n",
			order->item_quantities[i],
			order->item_codes[i]);
	}
}

void print_receipt(Order *order, Menu *menu)
{
	for (int i = 0; i < order->num_items; i++)
	{
		double item_cost = get_item_cost(order->item_codes[i], menu);
		fprintf(
			stdout,
			"%d x (%s)\n @$%.2f ea \t %.2f\n",
			order->item_quantities[i],
			order->item_codes[i],
			item_cost,
			item_cost * order->item_quantities[i]);
	}
	double order_subtotal = get_order_subtotal(order, menu);
	double order_total = get_order_total(order, menu);

	fprintf(stdout, "Subtotal: \t %.2f\n", order_subtotal);
	fprintf(stdout, "               -------\n");
	fprintf(stdout, "Tax %.2f%%: \t$%.2f\n", TAX_RATE, order_total);
	fprintf(stdout, "              ========\n");
}