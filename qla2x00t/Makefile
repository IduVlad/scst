qla2xxx-y := qla_os.o qla_init.o qla_mbx.o qla_iocb.o qla_isr.o qla_gs.o \
		qla_dbg.o qla_sup.o qla_attr.o qla_mid.o qla_dfs.o

obj-$(CONFIG_SCSI_QLA_FC) += qla2xxx.o

clean:
	rm -f *.o *.ko .*.cmd *.mod.c .*.d .depend *~ Modules.symvers \
		Module.symvers Module.markers modules.order
	rm -rf .tmp_versions

extraclean: clean
	rm -f *.orig *.rej

.PHONY: clean extraclean
