/**
 * File     : errifiergui.cpp
 * Purpose  : The file contains various implementations of the GUI such as when browse button
 *            IS CLICKED, or when "Add to Watch List" button is CLICKED, when we want to
 *            change the status of error reporting(Active/Paused) by double clicking, or seeing
 *            the last logged errors.
 *
 * @author : Ayush Choubey(ayush.choubey@gmail.com)
 * @version: 1.0
 */


#define QT_NO_KEYWORDS

#include "errifiergui.h"
#include "ui_errifiergui.h"

#include <sys/inotify.h>
#include <libnotify/notify.h>
#include <pthread.h>
#include <QFileInfo>
#include <QMessageBox>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

static void * startWatch(void *funcargs); //Function Prototype

/**
 * Constructor. Used to set Table view
 */
ErrifierGUI::ErrifierGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ErrifierGUI)
{
    ui->setupUi(this);

    //Start setting TableView, where data can be displayed.
    model = new QStandardItemModel(this);
    //Initialising three headers of the table.
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("File Location")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Friendly Name")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Status")));

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    // Initialising the threadcount
    threads.threadcount = 0;

    // Makes the window of standard size, So that it can't be maximised or resized
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(width(), height());

}

ErrifierGUI::~ErrifierGUI()
{
    delete ui;
}

/**
 * When browse button is clicked. Systems file open window is opened which
 * helps to select file which are to be monitored
 */
void ErrifierGUI::on_browse_clicked()
{
    QString fileNames = QFileDialog::getOpenFileName(this, tr("Open File"),"/",tr("All Files (*.*)"));
    this->ui->txt_file->setText(fileNames);
}

/**
 *  Checks whether the credentials entered are already present or not
 *
 *  @param fileloc      Location of the File in char *
 *  @param friendlyName Friendly Name used to differentiate between services
 *  @param ui           Pointer to UI
 *
 *  @return int
 */
int fileAlreadyMonitored(const char *fileloc, const char*friendlyName, Ui::ErrifierGUI *ui)
{
    QAbstractItemModel *model = ui->tableView->model();
    int rowCount = model->rowCount();
    int i = 0;
    int ispresent = 0;
    for(i=0;i<rowCount;i++){
        const char *fstcoldata = model->index(i,0).data().toString().toUtf8().constData();
        const char *seccoldata = model->index(i,1).data().toString().toUtf8().constData();

        if((strcmp(fstcoldata, fileloc) == 0) || (strcmp(seccoldata, friendlyName) == 0)) {
            ispresent = 1;
            break;
        }
    }
    return ispresent;
}

/**
 * When "Add to Watch List" button is clicked, Take the filename and friendlyName.
 * Create a thread and start monitoring the file, and add the details to tha table
 */
void ErrifierGUI::on_watch_clicked()
{
    //Take FileName
    QString fileloc = this->ui->txt_file->text();
    QString friendlyName = this->ui->txt_friendly->text();
    char *filelocString = fileloc.toUtf8().data();
    char *friendlyNameString = friendlyName.toUtf8().data();
    int ifpresent = 0;

    QFileInfo fileinfo(fileloc);

    if(fileinfo.isFile() && friendlyName.length() > 0 && !(ifpresent = fileAlreadyMonitored(filelocString, friendlyNameString, this->ui))) {
        //If its a valid file and a friendly name has been provided and its already not being monitored
        WatcherArgList *args = new WatcherArgList();

        int fd;
        int wd;

        fd = inotify_init();
        if ( fd < 0 ) {
            perror( "inotify_init" );
        }

        //Use Inotify to watch the file for any modifications

        wd = inotify_add_watch( fd, filelocString,
                                IN_MODIFY );

        //Start setting the argument object which is to be passed to the thread
        strcpy(args->filename, filelocString);
        strcpy(args->friendlyname, friendlyNameString);
        args->fd = fd;
        threads.mutexthread[threads.threadcount] = 1; //set the mutex as 1 that is thread should run
        args->toWatch = &threads.mutexthread[threads.threadcount];

        //Create a thread and start monitoring file for any changes
        pthread_create (&threads.thread[threads.threadcount], NULL, &startWatch, args);
        threads.threadcount++;

        this->ui->txt_file->setText("");
        this->ui->txt_friendly->setText("");

        //Add the files description to the table as new Row
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem(fileloc));
        newRow.append(new QStandardItem(friendlyName));
        newRow.append(new QStandardItem(QString("Active")));
        model->appendRow(newRow);
    }
    else if(!fileinfo.isFile()) {
        //If its not a valid file, display a message box and setFocus

        QMessageBox::warning(NULL, "File Name Error",
                             "Are you sure that's a correct filename");
        this->ui->txt_file->setFocus();
    }
    else if(friendlyName.length() == 0){
        //If friendly name is not provided, then display a message box and setFocus

        QMessageBox::warning(NULL, "Friendly Name",
                             "Please enter a friendly name. This friendly name will help you differentiate between your services.");
        this->ui->txt_friendly->setFocus();
    }
    else if(ifpresent == 1) {
        //If File is already being monitored or friendly Name is already present

        QMessageBox::warning(NULL, "Correct Input",
                             "There seems to be a duplicated data. Either friendly Name or the filename is already present ");
        this->ui->txt_file->setFocus();
    }
}

/**
 * Function that is passed as thread. It monitors the file continously
 * and if there is any modification, then a notification is sent to
 * the users screen.
 *
 * @param  funcargs The argument object which is passed when thread was created. Refer to WatcherArgList for information
 * @return void
 */
static void * startWatch(void *funcargs)
{
    int length, i = 0;
    char buffer[BUF_LEN];
    char msg[200];

    WatcherArgList * args = (WatcherArgList *) funcargs;

    char *friendlyname = args->friendlyname;
    int fd = args->fd;

    int toWatch;

    while(1){

        //toWatch takes value from the mutex array of the thread field.
        //Its value can be changed when third column of the table is double clicked.
        //It is responsible for making the thread active or paused
        toWatch = *(args->toWatch);

        if(toWatch){  //If file has to be watched.
            length = read( fd, buffer, BUF_LEN );

            if ( length < 0 ) {
              perror( "read" );
            }

            while ( length > 0 && i < length ) {
                struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];

                if ( event->mask & IN_MODIFY ) {
                    notify_init (friendlyname);

                    //Start Creating the message that should be pop-uped
                    strcpy(msg,"There is error in ");
                    strcat(msg,friendlyname);
                    strcat(msg," .Please Check your logs");

                    NotifyNotification * Hello = notify_notification_new ("Error", msg, "dialog-error");

                    notify_notification_show (Hello, NULL);
                    g_object_unref(G_OBJECT(Hello));
                    notify_uninit();

                    //Clear the msg variable
                    msg[0] = '\0';
                }

                i += EVENT_SIZE + event->len;
            }
            i=0;
        }
    }
    return NULL; //Don't care about the output
}

/**
 * When a cell in the table is double clicked
 *
 * @param  index Keeps track of which cell was clicked
 * @return void
 */
void ErrifierGUI::on_tableView_doubleClicked(const QModelIndex &index)
{
    char *val;
    switch(index.column())
    {
        case 0: //If first column was Clicked. Then we need to show the user the log file
        {
            //Just do a basic tail command, get the output and print it in a textbox
            //I could've implemented the entire tail command, but then i thought why not re-use the function

            char command[500] = "tail -5 ";
            int i = 5; //We need just last 5 logs. To change the value, change the value of i and the command above

            QPlainTextEdit *txt = new QPlainTextEdit();

            val = index.data().toString().toUtf8().data();


            strcat(command,val);

            FILE* file = popen(command, "r");
            char *buffer = new char[2048];
            char *totalBuf = new char[2048*5];

            strcpy(totalBuf,"");

            //start reading the output line by line into buffer variable and concatinate them in totalBuf
            while(i--) {
                fgets(buffer, 2048, file);
                strcat(totalBuf, buffer);
                strcat(totalBuf, "\r\n\r\n");
            }

            pclose(file);

            txt->appendPlainText(QString(totalBuf)); //Add it to the textbox

            txt->showMaximized(); //Make Textbox Visible

            delete(buffer);
            delete(totalBuf);

            break;
        }
        case 2: //If 3rd column is double clicked, then pause/Activate the thread

        /** The idea is pretty simple. The row in a table matches to the index of the thread in threadList.
         *  So, we take the row id and then use that as an index of the mutexThread in threadlist change the
         *  mutex value, which actually determines, whether notification should appear or not
         */
            int rowid = index.row();
            val = index.data().toString().toUtf8().data();

            if(strcmp(val,"Active") == 0) { //If its Active, Change it to Paused
                threads.mutexthread[rowid] = 0;
                this->ui->tableView->model()->setData(index, "Paused");
            }
            else { //If its in Paused State, Change it to active
                threads.mutexthread[rowid] = 1;
                this->ui->tableView->model()->setData(index, "Active");
            }

    }
}
