#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mpi.h"
#include "hdf5.h"

#define NX     16                   
#define NY     8
#define RANK   2



int main (int argc, char **argv){

  const char filename[] = "eff_file.h5";
  hid_t file_id;
  hid_t dataspace1, dataspace2;
  hid_t dataspace3, dataspace4;
  hid_t dataspace5, dataspace6;
  hid_t dataspace7, dataspace8;
  hid_t dset_id;
  hid_t fapl_id, dxpl_id;
  
  const unsigned int nelem = 60;
  int *data = NULL, *data1 = NULL;
  int *data2 = NULL, *data3 = NULL;
  unsigned int i = 0;
  hsize_t dimsf[2];
  hsize_t count[2], offset[2];
  int my_rank, my_size, ret;
  int provided;
  hid_t event_q, int_id;
  int num_requests = 0;
  H5_status_t *status = NULL;
  MPI_Comm comm;
  MPI_Info info;

  
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  if(MPI_THREAD_MULTIPLE != provided) {
    fprintf(stderr, "MPI does not have MPI_THREAD_MULTIPLE support\n");
    exit(1);
  }

  comm = MPI_COMM_WORLD;
  info = MPI_INFO_NULL;
  
  EFF_init (comm, info);
  MPI_Comm_rank (comm, &my_rank);
  MPI_Comm_size (comm, &my_size);

  if (my_size > 1){
    fprintf(stderr, "APP processes = %d cannot be greater than 1 \n", my_size);
    EFF_finalize();
    MPI_Finalize();
  }

  fapl_id = H5Pcreate (H5P_FILE_ACCESS);
  H5Pset_fapl_iod(fapl_id, comm, info);

  event_q = H5EQcreate(fapl_id);
  assert(event_q);

  file_id = H5Fcreate_ff(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id, event_q);
  assert(file_id);
  
  dimsf[0] = NX;
  dimsf[1] = NY;

  dataspace1 = H5Screate_simple(RANK, dimsf, NULL);
  dataspace3 = H5Screate_simple(RANK, dimsf, NULL);
  dataspace5 = H5Screate_simple(RANK, dimsf, NULL);
  dataspace7 = H5Screate_simple(RANK, dimsf, NULL);
  
  dset_id  = H5Dcreate_ff (file_id, "D1", H5T_NATIVE_INT, dataspace1, 
			   H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT, 0, event_q);
  assert(dset_id);
  
  count[0] = 4;
  count[1] = 8;
  offset[0] = 0;
  offset[1] = 0;

  dataspace2 = H5Screate_simple(RANK, count, NULL);
  	
  H5Sselect_hyperslab(dataspace1, 
		      H5S_SELECT_SET,
		      offset,
		      NULL,
		      count,
		      NULL);
  data = (int *) malloc (sizeof(int)*count[0]*count[1]);

  for (i = 0; i < count[0]*count[1]; i++){
    data[i] = 10;
  }
  
  ret = H5Dwrite_ff(dset_id,
		    H5T_NATIVE_INT,
		    dataspace2,
		    dataspace1,
		    H5P_DEFAULT,
		    data,
		    0,
		    event_q);
  assert(ret == 0);


  count[0] = 4;
  count[1] = 8;
  offset[0] = 4;
  offset[1] = 0;



  data1 = (int *) malloc(sizeof(int)*count[0]*count[1]);
  for (i = 0; i < count[0]*count[1]; i++){
    data1[i] = 11;
  }
  
  dataspace4 = H5Screate_simple(RANK, count, NULL);

  H5Sselect_hyperslab(dataspace3, 
		      H5S_SELECT_SET,
		      offset,
		      NULL,
		      count,
		      NULL);

  
  ret = H5Dwrite_ff(dset_id,
		    H5T_NATIVE_INT,
		    dataspace4,
		    dataspace3,
		    H5P_DEFAULT,
		    data1,
		    0,
		    event_q);
  assert(ret == 0);
  

  count[0] = 2;
  count[1] = 8;
  offset[0] = 5;
  offset[1] = 0;

  data3 = (int *) malloc(sizeof(int)*count[0]*count[1]);
  for (i = 0; i < count[0]*count[1]; i++){
    data3[i] = 100;
  }

  dataspace8 = H5Screate_simple(RANK, count, NULL);

  H5Sselect_hyperslab(dataspace7, 
		      H5S_SELECT_SET,
		      offset,
		      NULL,
		      count,
		      NULL);



  ret = H5Dwrite_ff(dset_id,
		    H5T_NATIVE_INT,
		    dataspace8,
		    dataspace7,
		    H5P_DEFAULT,
		    data3,
		    0,
		    event_q);
  assert(ret == 0);


  count[0] = 8;
  count[1] = 8;
  offset[0] = 0;
  offset[1] = 0;


  dataspace6 = H5Screate_simple(RANK, count, NULL);

  H5Sselect_hyperslab(dataspace5, 
		      H5S_SELECT_SET,
		      offset,
		      NULL,
		      count,
		      NULL);




  data2 = (int *) malloc(sizeof(int)*count[0]*count[1]);

  ret = H5Dread_ff(dset_id, 
		   H5T_NATIVE_INT,
		   dataspace6, 
		   dataspace5,
		   H5P_DEFAULT,
		   data2,
		   0,
		   event_q);
  assert(ret == 0);




  assert(H5Dclose_ff(dset_id, event_q) == 0);
  H5Sclose(dataspace1); H5Sclose(dataspace2);
  H5Sclose(dataspace3); H5Sclose(dataspace4);
  H5Sclose(dataspace5); H5Sclose(dataspace6);
  H5Pclose(fapl_id);
  assert(H5Fclose_ff(file_id, event_q)== 0);
  H5EQwait(event_q, &num_requests, &status);
  free(status);
  H5EQclose (event_q);

  for (i = 0; i < (count[0] * count[1]); i++){
    fprintf (stderr,"data2[%d]: %d\n",
	     i, data2[i]);
  }

  free(data);
  free(data1);
  free(data2);
  free(data3);
  fprintf(stderr, "\n*****************************************************************************************************************\n");
  fprintf(stderr, "Finalize EFF stack\n");
  fprintf(stderr, "*****************************************************************************************************************\n");
  
  EFF_finalize();
  MPI_Finalize();
  
  return 0;
}
