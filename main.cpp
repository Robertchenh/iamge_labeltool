#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <dirent.h>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME "image_label_working"

using namespace std;
void Label_Update(int & count_, int * label_select_, int & label_value_, bool & SELECT_, string & label_, int select_, int number_);
void get_iamge_xy(int left_window_size_, int image_width_, int image_height_, int & image_x_, int & image_y_);
double get_image_scale(int image_width_, int image_height_);
vector<string> getFiles(string cate_dir);
void get_mouse_xy(int image_x_, int image_y_, int cursor_x_, int cursor_y_, int & mouse_x_, int & mouse_y_);
void label_fun(cv::Mat & frame_, const int mouse_x_, const int mouse_y_, const int image_cols_, const int image_rows_,const int image_x_, const int image_y_);
bool findFiles(const string file_name_, const string label_file_path_);
void show_label(cv::Mat & frame_);


string LABELS[] = {"__background__",
                         "aeroplane", "bicycle", "bird", "boat",
                         "bottle", "bus", "car", "cat", "chair",
                         "cow", "diningtable", "dog", "horse",
                         "motorbike", "person", "pottedplant",
                         "sheep", "sofa", "train", "tvmonitor",};

struct object
{
    int object_id;
    string object_name;
    int object_boundboxs[4];
};

class Image_info
{
private:
    string image_name;
    int image_size[3];
    vector<object> objects;

public:
    vector<object> get_info_from_file(const string filename_, const string filepath_, const double image_scale_);
    void save_file(const string & filename_, const string & filepath_,const double image_scale_);
    void redraw(const string & filename_, const string & filepath_);
    void turn_into_xml();
    void show_info(cv::Mat & frame, const int right_window_size, const int up_padding, const string filename, const int & image_x, const int & image_y);
    void initial_info();
    void set_name(const string name_);
    void set_size(const int weidth_, const int height_, const int channel_);
    void set_objects(const vector<object> object_);
    void add_boject(const object object_);
};



int main(int argc, const char *argv[])
{
    bool SAVE = false;
    bool QUIT = false;
    bool REDRAW = false;
    bool GAIN_OBJECT = false;
    bool XML = false;
    bool LAST_IMAGE = false;
    bool NEXT_IMAGE = false;
    bool SELECT_0 = false;
    bool SELECT_1 = false;
    bool SELECT_2 = false;
    bool SELECT_3 = false;
    bool SELECT_4 = false;
    bool SELECT_5 = false;
    bool SELECT_6 = false;
    bool SELECT_7 = false;
    bool SELECT_8 = false;
    bool SELECT_9 = false;
    bool SELECT_repeal = false;
    bool CHANG_IMANG = false;
    bool RESTART = false;
    bool file_exist = false;
    bool chang_power = false;
    bool mouse_click = false;
    bool rectangle = true;


    int left_padding = 20;
    int up_padding = 30;
    int left_window_size = 200;
    int right_window_size = 400;
    int left_button_height = 40;
    int left_button_width = 140;
    int left_button_padding = 50;
    int number = sizeof(LABELS) / sizeof(LABELS[0]);
    int label_select[4] = {0, 0, 0, 0};
    int label_value = 0;
    int count = 0;
    int image_x=0, image_y=0, image_width, image_height;
    int mouse_status;
    int mouse_status_label;
    int mouse_x, mouse_y;
    int mouse_clicks = 0;
    int object_id = 0;
    int object_boundbox[4] = {0, 0, 0, 0};

    long image_id = 0;

    double image_scale = 1.0;
    string image_name;
    string label = "you select nothing!";
    string file_path;
    string file_path_ = "/";
    string notice = "If you forget the label number, please put your mouse over me for more help!!!";

    char basePath[100];
    memset(basePath, '\0', sizeof(basePath));
    getcwd(basePath, 999);
    string basePath_ = basePath;
    string DataPath = basePath_ + "/image_datas";
    string label_file_path = basePath_ + "/image_labels";

    cout << "the Data_path is： "<< DataPath<<endl;
    vector<string> files = getFiles(DataPath);

    object object_info;
    vector<object> objects_info;
    Image_info image_info;

    cv::Mat frame = cv::Mat(700, 1300, CV_8UC3);
    cv::Mat image;
    cv::Point cursor;
    cvui::init(WINDOW_NAME);

    while (true)
    {
        frame = cv::Scalar(128, 138, 135);
        SAVE = cvui::button(frame, left_padding, up_padding+ 2 * left_button_padding, left_button_width, left_button_height, "Save");
        QUIT = cvui::button(frame, left_padding, up_padding + 0 * left_button_padding, left_button_width, left_button_height, "Quit");
        REDRAW = cvui::button(frame, left_padding, up_padding + 1 * left_button_padding, left_button_width, left_button_height, "Redraw");
        XML = cvui::button(frame, left_padding, up_padding + 3 * left_button_padding, left_button_width, left_button_height, "Turn into XML");

        LAST_IMAGE = cvui::button(frame, left_padding, up_padding + 5 * left_button_padding, left_button_width, left_button_height, "Last Image");
        NEXT_IMAGE = cvui::button(frame, left_padding, up_padding + 6 * left_button_padding, left_button_width, left_button_height, "Next Image");
        RESTART = cvui::button(frame, left_padding, up_padding + 7 * left_button_padding, left_button_width, left_button_height, "RESTART");
        GAIN_OBJECT = cvui::button(frame, left_padding, up_padding + 8 * left_button_padding, left_button_width, left_button_height, "Gain The Object");

        cvui::text(frame, left_padding, up_padding + 8 * left_button_padding + 50, "Label Number:", 0.6, 0xff0000);
        SELECT_0 = cvui::checkbox(frame, left_padding, up_padding + 8 * left_button_padding + 80, "0", &SELECT_0, 0xff0000);
        SELECT_1 = cvui::checkbox(frame, left_padding + 50, up_padding + 8* left_button_padding + 80, "1", &SELECT_1, 0xff0000);
        SELECT_2 = cvui::checkbox(frame, left_padding + 100, up_padding + 8 * left_button_padding + 80, "2", &SELECT_2, 0xff0000);
        SELECT_3 = cvui::checkbox(frame, left_padding, up_padding + 8.5 * left_button_padding + 80, "3", &SELECT_3, 0xff0000);
        SELECT_4 = cvui::checkbox(frame, left_padding + 50, up_padding + 8.5 * left_button_padding + 80, "4", &SELECT_4, 0xff0000);
        SELECT_5 = cvui::checkbox(frame, left_padding + 100, up_padding + 8.5 * left_button_padding + 80, "5", &SELECT_5, 0xff0000);
        SELECT_6 = cvui::checkbox(frame, left_padding, up_padding + 9 * left_button_padding + 80, "6", &SELECT_6, 0xff0000);
        SELECT_7 = cvui::checkbox(frame, left_padding + 50, up_padding + 9 * left_button_padding + 80, "7", &SELECT_7, 0xff0000);
        SELECT_8 = cvui::checkbox(frame, left_padding + 100, up_padding + 9 * left_button_padding + 80, "8", &SELECT_8, 0xff0000);
        SELECT_9 = cvui::checkbox(frame, left_padding, up_padding + 9.5 * left_button_padding + 80, "9", &SELECT_9, 0xff0000);
        SELECT_repeal = cvui::checkbox(frame, left_padding + 50, up_padding + 9.5 * left_button_padding + 80, "repeal", &SELECT_repeal, 0xff0000);

        if(LAST_IMAGE or NEXT_IMAGE or RESTART or REDRAW)
        {
            object_boundbox[0] = 0;
            object_boundbox[1] = 0;
            object_boundbox[2] = 0;
            object_boundbox[3] = 0;
            objects_info.clear();
            object_id = 0;
            mouse_clicks = 0;
            image_info.set_name(files[image_id]);
        }

        if((chang_power) and object_id)
        {
            LAST_IMAGE = false;
            NEXT_IMAGE = false;
            RESTART = false;
        }



        if(count == 0)
        {
            cvui::text(frame, left_padding, up_padding + 10 * left_button_padding + 80, "Number is:", 0.5, 0xff0000);
        }
        else
        {
            cvui::printf(frame, left_padding, up_padding + 10 * left_button_padding + 80, 0.5, 0xff0000, "Number is: %d", label_value);
        }
        cvui::text(frame, left_padding, up_padding + 10.5 * left_button_padding + 80, label, 0.6, 0xff0000);

        cvui::text(frame, left_padding, up_padding + 10.5 * left_button_padding + 120, notice, 0.6, 0x284f27);

//      image
        if(NEXT_IMAGE)
        {
            image_id ++;

            if(image_id > files.size() - 1)
            {
                image_id = files.size() - 1;
                notice = "This is the last image!!!";
            }
            else notice = "If you forget the label number, please put your mouse over me for more help!!!";
            NEXT_IMAGE = false;
            CHANG_IMANG = true;
            if(object_id == 0)
            {
                rectangle = false;
            }

        }

        if(LAST_IMAGE)
        {
            image_id --;
            if(image_id < 0)
            {
                image_id = 0;
                notice = "This is the first image!!!";
            }
            else notice = "If you forget the label number, please put your mouse over me for more help!!!";
            LAST_IMAGE = false;
            CHANG_IMANG = true;
            if(object_id == 0)
            {
                rectangle = false;
            }
        }

        if(RESTART)
        {
            image_id = 0;
            LAST_IMAGE = false;
            CHANG_IMANG = true;
            RESTART = false;
            if(object_id == 0)
            {
                rectangle = false;
            }
        }

        file_path = DataPath + file_path_ + files[image_id];



        if(image_id == 0 or CHANG_IMANG)
        {
            image = cv::imread(file_path, cv::IMREAD_COLOR);
            image_width = image.cols;
            image_height = image.rows;
            image_scale = get_image_scale(image_width, image_height);
            cv::resize(image, image, cv::Size(image_width * image_scale, image_height * image_scale), 0, 0, cv::INTER_LINEAR);
            get_iamge_xy(left_window_size, image_width * image_scale, image_height * image_scale, image_x, image_y);
            string file_name = files[image_id].substr(0, files[image_id].find('.')) + ".txt";
            file_exist = findFiles(file_name, label_file_path);
            if(not file_exist)
            {
                if(object_id == 0) image_info.initial_info();
                image_info.set_name(files[image_id]);
//                object_id = 0;
                image_info.set_size(image_width * image_scale, image_height * image_scale, image.channels());
            }
            else
            {
                if(object_id == 0)
                {
                    objects_info = image_info.get_info_from_file(file_name, label_file_path, image_scale);
                }
                object_id = objects_info.size();
                image_info.set_name(files[image_id]);
                image_info.set_size(image_width * image_scale, image_height * image_scale, image.channels());
            }

        }

        cvui::image(frame, image_x, image_y, image);


//      __end



//      Label information
        cvui::window(frame, 1300 - right_window_size + 20, up_padding, 360, 640, "Image Label Information");

//      __end;

//      Label Number;
        if(SELECT_0 or SELECT_1 or SELECT_2 or SELECT_3 or SELECT_4 or SELECT_5 or SELECT_6 or SELECT_7 or SELECT_8 or SELECT_9)
        {
            count ++;
        }

        if(SELECT_repeal or CHANG_IMANG)
        {
            SELECT_repeal = false;
            label = "you select nothing!";
            count = 0;
            label_select[0] = 0;
            label_select[1] = 0;
            label_select[2] = 0;
            label_select[3] = 0;
            CHANG_IMANG = false;
        }

        if(SELECT_0)
        {

            Label_Update(count, label_select, label_value, SELECT_0, label, 0, number);

//            if(count == 1)
//            {
//                label = "wrong number!";
//                count = 0;
//                label_select[0] = 0;
//                label_select[1] = 0;
//                label_select[2] = 0;
//                label_select[3] = 0;
//                SELECT_0 = false;
//            }
//            else
//            {
//                Label_Update(count, label_select, label_value, SELECT_0, label, 0, number);
//            }

        }
        if(SELECT_1)
        {
            Label_Update(count, label_select, label_value, SELECT_1, label, 1, number);
        }
        if(SELECT_2)
        {
            Label_Update(count, label_select, label_value, SELECT_2, label, 2, number);
        }
        if(SELECT_3)
        {
            Label_Update(count, label_select, label_value, SELECT_3, label, 3, number);
        }
        if(SELECT_4)
        {
            Label_Update(count, label_select, label_value, SELECT_4, label, 4, number);
        }
        if(SELECT_5)
        {
            Label_Update(count, label_select, label_value, SELECT_5, label, 5, number);
        }
        if(SELECT_6)
        {
            Label_Update(count, label_select, label_value, SELECT_6, label, 6, number);
        }
        if(SELECT_7)
        {
            Label_Update(count, label_select, label_value, SELECT_7, label, 7, number);
        }
        if(SELECT_8)
        {
            Label_Update(count, label_select, label_value, SELECT_8, label, 8, number);
        }
        if(SELECT_9)
        {
            Label_Update(count, label_select, label_value, SELECT_9, label, 9, number);
        }

//        if(count >= 4)
//        {
//            label = "wrong number, please again!";
//            count = 0;
//            label_select[0] = 0;
//            label_select[1] = 0;
//            label_select[2] = 0;
//            label_select[3] = 0;
//        }

        if(not(label == "you select nothing!") and not(label == "wrong number, please again!") and not(label == "wrong number!"))
        {
            chang_power = true;
        }

//      ____end;


//      label
        mouse_status = cvui::iarea(image_x, image_y, image.cols, image.rows);
//        cvui::rect(frame, image_x, image_y, image.cols, image.rows, 0xff0000);
        cursor = cvui::mouse();
        get_mouse_xy(image_x, image_y, cursor.x, cursor.y, mouse_x, mouse_y);


        switch (mouse_status)
        {

            case cvui::CLICK:
            {
                mouse_clicks ++;
                cvui::printf(frame, cursor.x + 5, cursor.y - 15, "(%d, %d)", mouse_x, mouse_y);
                label_fun(frame, cursor.x, cursor.y, image.cols, image.rows, image_x, image_y);

                mouse_clicks = mouse_clicks % 2;

                if(mouse_clicks == 1)
                {
                    object_boundbox[0] = mouse_x;
                    object_boundbox[1] = mouse_y;
                    object_boundbox[2] = mouse_x;
                    object_boundbox[3] = mouse_y;
                    mouse_click = true;
                    rectangle = true;
                }
                if(mouse_clicks == 0)
                {
                    object_boundbox[2] = mouse_x;
                    object_boundbox[3] = mouse_y;
                    cout<<object_boundbox[0]<<"     "<<object_boundbox[1]<<"     "<<object_boundbox[2]<<"     "<<object_boundbox[3]<<"     "<<endl;
                    mouse_click = false;

                }

                if((mouse_clicks == 1) and mouse_click and rectangle)
                {
                    cv::rectangle(frame, cvPoint(object_boundbox[0] + image_x, object_boundbox[1] + image_y), cvPoint(cursor.x, cursor.y), cv::Scalar(0, 0, 255), 1.5, 1, 0);
                }
                break;
            }
            case cvui::DOWN:
                {
                    cvui::printf(frame, cursor.x + 5, cursor.y - 15, "(%d, %d)", mouse_x, mouse_y);
                    label_fun(frame, cursor.x, cursor.y, image.cols, image.rows, image_x, image_y);
                    if((mouse_clicks == 1) and mouse_click and rectangle)
                    {
                        cv::rectangle(frame, cvPoint(object_boundbox[0] + image_x, object_boundbox[1] + image_y), cvPoint(cursor.x, cursor.y), cv::Scalar(0, 0, 255), 1.5, 1, 0);
                    }
                    break;
                }

            case cvui::OVER:
            {
                cvui::printf(frame, cursor.x + 5, cursor.y - 15, "(%d, %d)", mouse_x, mouse_y);
                label_fun(frame, cursor.x, cursor.y, image.cols, image.rows, image_x, image_y);

                if((mouse_clicks == 1) and mouse_click and rectangle)
                {
                    cv::rectangle(frame, cvPoint(object_boundbox[0] + image_x, object_boundbox[1] + image_y), cvPoint(cursor.x, cursor.y), cv::Scalar(0, 0, 255), 1.5, 1, 0);
                }
                break;
            }
            case cvui::OUT: break;
        }

        if((object_boundbox[0] != 0 or object_boundbox[1] != 0 or object_boundbox[0] != 0 or object_boundbox[0] != 0)
           and (object_boundbox[0] != object_boundbox[2]) and (not GAIN_OBJECT))
        {
            cv::rectangle(frame, cvPoint(object_boundbox[0] + image_x, object_boundbox[1] + image_y), cvPoint(object_boundbox[2]
                                                    + image_x, object_boundbox[3] + image_y), cv::Scalar(0, 0, 255), 1.5, 1, 0);
        }

        if(chang_power and GAIN_OBJECT and (object_boundbox[0] != object_boundbox[2]))
        {
            object object_info_ = {object_id, label, {object_boundbox[0],object_boundbox[1],object_boundbox[2],object_boundbox[3]}};
            objects_info.push_back(object_info_);
            image_info.set_objects(objects_info);
            object_id ++;
            object_boundbox[0] = 0;
            object_boundbox[1] = 0;
            object_boundbox[2] = 0;
            object_boundbox[3] = 0;
            chang_power = false;
            SELECT_repeal = true;
        }


        if(REDRAW)
        {
            image_info.redraw(files[image_id], label_file_path);
            image_info.initial_info();
        }



//      __end;



        image_info.show_info(frame, right_window_size, up_padding, files[image_id], image_x, image_y);
        if(SAVE)
        {
            image_info.save_file(files[image_id], label_file_path, image_scale);
        }
//      image box
//        for(int i=0; i < objects_info.size(); i++)
//        {
//            cvui::rect(frame, objects_info[i].object_boundboxs[0] + image_x, objects_info[i].object_boundboxs[1] + image_y, objects_info[i].object_boundboxs[2]
//                                    - objects_info[i].object_boundboxs[0], objects_info[i].object_boundboxs[3] - objects_info[i].object_boundboxs[1], 0x836FFF);
//            cvui::text(frame, objects_info[i].object_boundboxs[0] + image_x, objects_info[i].object_boundboxs[1] + image_y - 15, objects_info[i].object_name, 0.4, 0x0000FF);
//        }
//      __end

//      right_info
//        cvui::printf(frame, 1300 - right_window_size + 20 + 10, up_padding + 25 , 0.4, 0xFFFF00, " Image Name :  %s", files[image_id].data());
//        cvui::printf(frame, 1300 - right_window_size + 20 + 10, up_padding + 50 , 0.4, 0xff0000, " Id   Object Name     X1     Y1     X2     Y2");
//
//        for(int i=0; i < objects_info.size(); i++)
//        {
//            cvui::printf(frame, 1300 - right_window_size + 20 + 16, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects_info[i].object_id);
//            cvui::text(frame, 1300 - right_window_size + 20 + 16 + 31, up_padding + 50 + 15 + 15 * i, objects_info[i].object_name, 0.4, 0xFFFF00);
//            cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 144, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects_info[i].object_boundboxs[0]);
//            cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 192, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects_info[i].object_boundboxs[1]);
//            cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 240, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects_info[i].object_boundboxs[2]);
//            cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 287, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects_info[i].object_boundboxs[3]);
//        }





//      get label_info
        mouse_status_label = cvui::iarea(left_padding, up_padding + 10.5 * left_button_padding + 120, 800, 15);

        switch (mouse_status_label)
        {
            case cvui::CLICK: break;
            case cvui::DOWN: break;
            case cvui::OUT: break;
            case cvui::OVER:
            {
                show_label(frame);
                cvui::text(frame, left_padding, up_padding + 10.5 * left_button_padding + 120, notice, 0.6, 0xFFFF00);
                break;
            }
        }

//      __end

        if(QUIT)
        {
            break;
        }

        cvui::imshow(WINDOW_NAME, frame);

        if (cv::waitKey(21) == 27)
        {
            break;
        }
    }
        return 0;

}



void  Label_Update(int & count_, int * label_select_, int & label_value_, bool & SELECT_, string & label_, int select_, int number_)
{
    for(int i=1; i < count_; i++)
    {
        label_select_[count_ -i] = label_select_[count_ - (i+1)];
    }
    label_select_[0] = select_;
    label_value_ = label_select_[0] + 10 * label_select_[1] + 100 * label_select_[2] + 1000 * label_select_[3];
    if(label_value_ >= number_)
    {
        label_ = "wrong number, please again!";
        count_ = 0;
        label_select_[0] = 0;
        label_select_[1] = 0;
        label_select_[2] = 0;
        label_select_[3] = 0;
    }
    else
    {
        label_ = LABELS[label_value_];
    }
    SELECT_ = false;
}

void get_iamge_xy(int left_window_size_, int image_width_, int image_height_, int & image_x_, int & image_y_)
{
    int center_x = left_window_size_ + 340;
    int center_y = 350;
    image_x_ = center_x - 0.5 * image_width_;
    image_y_ = center_y - 0.5 * image_height_;
}


double get_image_scale(int image_width_, int image_height_)
{
    double image_scale = 640.0 / double(image_width_);
    if(image_scale * double(image_height_) > 640.)
    {
        image_scale = 640.0 / double(image_height_);
    }
    return image_scale;
}



vector<string> getFiles(string cate_dir)
{
    vector<string> files;
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(cate_dir.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
            continue;
        else if (ptr->d_type == 8)    ///file
            files.push_back(ptr->d_name);
        else if (ptr->d_type == 10)    ///link file
            continue;
        else if (ptr->d_type == 4)    ///dir
            continue;
    }
    closedir(dir);
    sort(files.begin(), files.end());
    return files;
}

bool findFiles(const string file_name_, const string label_file_path_)
{
    vector<string> files;
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(label_file_path_.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
            continue;
        else if (ptr->d_type == 8)    ///file
            files.push_back(ptr->d_name);
        else if (ptr->d_type == 10)    ///link file
            continue;
        else if (ptr->d_type == 4)    ///dir
            continue;
    }
    closedir(dir);
    sort(files.begin(), files.end());
    string file_name = file_name_;
    for(int i=0; i < files.size(); i++)
    {
        if(file_name == files[i])
            return true;
    }
    return false;
}

void get_mouse_xy(int image_x_, int image_y_, int cursor_x_, int cursor_y_, int & mouse_x_, int & mouse_y_)
{
    mouse_x_ = cursor_x_ - image_x_;
    mouse_y_ = cursor_y_ - image_y_;
}

void label_fun(cv::Mat & frame_, const int mouse_x_, const int mouse_y_, const int image_cols_, const int image_rows_, const int image_x_, const int image_y_)
{
    cv::Point start_0 = cv::Point(mouse_x_, image_y_);
    cv::Point end_0 = cv::Point(mouse_x_, image_y_ + image_rows_);
    cv::Point start_1 = cv::Point(image_x_, mouse_y_);
    cv::Point end_1 = cv::Point(image_x_ + image_cols_, mouse_y_);
    cv::line(frame_, start_0, end_0, cv::Scalar(0, 255, 255));
    cv::line(frame_, start_1, end_1, cv::Scalar(0, 255, 255));
}


void show_label(cv::Mat & frame_)
{
    cvui::window(frame_, 210, 0, 1070, 670, "Image Object Id information");
    sort(LABELS->begin(), LABELS->end());
    int object_number = sizeof(LABELS) / sizeof(LABELS[0]);
    int rows = ceil(double(object_number) / double(43));
    int show_ = 0;
    for(int row=0; row < rows; row++)
    {
        for(int j=0; j<43; j++)
        {
            cvui::printf(frame_, 210+10+row*150, 20 + j*15, 0.4, 0xff0000, "%d", show_);
            cvui::printf(frame_, 210+10+row*150 + 40, 20 + j*15, 0.4, 0xff0000, "%s", LABELS[show_].data());
            show_++;
            if(show_ == object_number)
            {
                break;
            }
        }
    }
}

vector<object> Image_info::get_info_from_file(const string filename_, const string filepath_, const double image_scale_)
{
    initial_info();
    string file_path_ = filepath_ + "/" + filename_;
    ifstream get_info;

    get_info.open(file_path_);
    if(not get_info.is_open())
    {
        cout<<"file open failed!"<<endl;
    }
    else
    {
        int k = 0;
        int key[5];
        int key_long[5];
        int object_id = 0;

        while (!get_info.eof())
        {
            char info[256];
            string object_name;
            string info_;
            int object_boundbox[4];
            object object_;
            get_info.getline(info, ' ');
            info_ = info;
            if(strlen(info) != 0)
            {
                int j = 0;
                if(k != 0)
                {
                    for(int i=0; i<strlen(info); i++)
                    {
                        if(info[i] == ' ')
                        {
                            key[j] = i;
                            key_long[j] = i - key[j-1];
                            j++;
                        }
                    }
                    object_id = atoi(info_.substr(0, key_long[0]).data());
                    object_name = info_.substr(key[0] + 1, key_long[1] - 1);
                    object_boundbox[0] = atoi(info_.substr(key[1] + 1, key_long[2] - 1).data()) * image_scale_;
                    object_boundbox[1] = atoi(info_.substr(key[2] + 1, key_long[3] - 1).data()) * image_scale_;
                    object_boundbox[2] = atoi(info_.substr(key[3] + 1, key_long[4] - 1).data()) * image_scale_;
                    object_boundbox[3] = atoi(info_.substr(key[4] + 1).data()) * image_scale_;
                    object_.object_id = object_id;
                    object_.object_name = object_name;
                    for(int m=0; m<4; m++)
                    {
                        object_.object_boundboxs[m] = object_boundbox[m];
                    }

                    objects.push_back(object_);


//                    cout<<"+++++++++++"<<object_id<<"+++++++++++"<<endl;
//                    cout<<"+++++++++++"<<object_name<<"+++++++++++"<<endl;
//                    cout<<"+++++++++++"<<object_boundbox[0]<<"+++++++++++"<<endl;
//                    cout<<"+++++++++++"<<object_boundbox[1]<<"+++++++++++"<<endl;
//                    cout<<"+++++++++++"<<object_boundbox[2]<<"+++++++++++"<<endl;
//                    cout<<"+++++++++++"<<object_boundbox[3]<<"+++++++++++"<<endl;
                }
            }

            k++;
        }
        get_info.close();
    }


    return objects;
}
void Image_info::save_file(const string & filename_, const string & filepath_ ,const double image_scale_)
{
    string file_name = filename_.substr(0, filename_.find('.')) + ".txt";
    bool file_exist = findFiles(file_name, filepath_);
    string file_path_ = filepath_ + "/" + file_name;
    if(file_exist)
    {
        bool delete_file = remove(file_path_.c_str());
        if(delete_file)
        {
            cout<<"delete file succeed!"<<endl;
        }

    }
//    mkdir(file_path_.c_str(), 777);
    ofstream write_file;
    write_file.open(file_path_);
    if(not write_file.is_open())
    {
        cout<<"found file failed!"<<endl;
    }
    else
    {
        cout<<"found file succeed!"<<endl;
    }
    write_file <<image_name<<" "<<image_size[0]<<" "<<image_size[1]<<" "<<image_size[2]<<endl;
    for(int i=0; i<objects.size(); i++)
    {
        write_file <<objects[i].object_id<<" "<<objects[i].object_name<<" "<<int(objects[i].object_boundboxs[0]/image_scale_)
                   <<" "<<int(objects[i].object_boundboxs[1]/image_scale_)<<" "<<int(objects[i].object_boundboxs[2]/image_scale_)
                   <<" "<<int(objects[i].object_boundboxs[3]/image_scale_)<<endl;
    }
    write_file.close();


}

void Image_info::redraw(const string & filename_, const string & filepath_)
{
    string file_name = filename_.substr(0, filename_.find('.')) + ".txt";
    bool file_exist = findFiles(file_name, filepath_);
    string file_path_ = filepath_ + "/" + file_name;
    if(file_exist)
    {
        bool delete_file = remove(file_path_.c_str());
        if(delete_file)
        {
            cout<<"delete file succeed!"<<endl;
        }

    }
    else
    {
        cout<<"Redraw failed!"<<endl;
    }
}
void Image_info::turn_into_xml()
{

}

void Image_info::show_info(cv::Mat & frame, const int right_window_size, const int up_padding, const string filename, const int & image_x, const int & image_y)
{

//      image box
    for(int i=0; i < objects.size(); i++)
    {
        cvui::rect(frame, objects[i].object_boundboxs[0] + image_x, objects[i].object_boundboxs[1] + image_y, objects[i].object_boundboxs[2]
                                                                                                              - objects[i].object_boundboxs[0], objects[i].object_boundboxs[3] - objects[i].object_boundboxs[1], 0x836FFF);
        cvui::text(frame, objects[i].object_boundboxs[0] + image_x, objects[i].object_boundboxs[1] + image_y - 15, objects[i].object_name, 0.4, 0x0000FF);
    }
//      __end

//      right_info
    cvui::printf(frame, 1300 - right_window_size + 20 + 10, up_padding + 25 , 0.4, 0xFFFF00, " Image Name :  %s", filename.data());
    cvui::printf(frame, 1300 - right_window_size + 20 + 10, up_padding + 50 , 0.4, 0xff0000, " Id   Object Name     X1     Y1     X2     Y2");

    for(int i=0; i < objects.size(); i++)
    {
        cvui::printf(frame, 1300 - right_window_size + 20 + 16, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects[i].object_id);
        cvui::text(frame, 1300 - right_window_size + 20 + 16 + 31, up_padding + 50 + 15 + 15 * i, objects[i].object_name, 0.4, 0xFFFF00);
        cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 144, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects[i].object_boundboxs[0]);
        cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 192, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects[i].object_boundboxs[1]);
        cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 240, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects[i].object_boundboxs[2]);
        cvui::printf(frame, 1300 - right_window_size + 20 + 16 + 287, up_padding + 50 + 15 + 15 * i, 0.4, 0xFFFF00, "%d", objects[i].object_boundboxs[3]);
    }
}

void Image_info::initial_info()
{
    image_name = '\0';
    image_size[0] = 0;
    image_size[1] = 0;
    image_size[2] = 0;
    objects.clear();
}

void Image_info::set_name(const string name_)
{
    image_name = name_;
}

void Image_info::set_size(const int weidth_, const int height_, const int channel_)
{
    image_size[0] = weidth_;
    image_size[1] = height_;
    image_size[2] = channel_;
}

void Image_info::set_objects(const vector<object> object_)
{
    objects = object_;
}

void Image_info::add_boject(const object object_)
{
    objects.push_back(object_);
}




