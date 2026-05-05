#include "LAS.hpp"

LAS *linear_address_space;

//===LAS prepare for Cluster interface===

unsigned Cluster::open(char *file_path) {
    ;
}

void Cluster::close(unsigned handle){
    ;
}

void Cluster::create(char *path){
    ;
}

void Cluster::delet(char *path){
    ;
}

void Cluster::show(){
    ;
}

void Cluster::command(char *cmd){
    ;
}

//===LAS===
LAS::LAS():
space{1}{
    ;
}

void LAS::regist(){
    ;
}

unsigned LAS::read(_WIN &handle, unsigned byte_offset, unsigned byte_read){
    if(handle.handle){
        Handle *h = reinterpret_cast<Handle *>(handle.handle);
        space[h->ID].drive->read(reinterpret_cast<unsigned short *>(&handle+1), h->file_handle, byte_offset, byte_read);
    }
    else {
        ;
    };
}

unsigned LAS::write(_WIN &handle, unsigned byte_offset, unsigned byte_write){
    if(handle.handle){
        Handle *h = reinterpret_cast<Handle *>(handle.handle);
        space[h->ID].drive->write(reinterpret_cast<unsigned short *>(&handle+1), h->file_handle, byte_offset, byte_write);
    }
    else {
        ;
    };
}

unsigned LAS::open(_WIN &, String file_path){
    ;
}

unsigned LAS::close(_WIN &){
    ;
}

unsigned LAS::mkdir(_WIN &, const char *){
    ;
}

unsigned LAS::deldir(_WIN &, const char *){
    ;
}

unsigned LAS::creat(_WIN &, const char *){
    ;
}

unsigned LAS::del(_WIN &, const char *){
    ;
}

unsigned LAS::cmd(_WIN &, const char *){
    ;
}

