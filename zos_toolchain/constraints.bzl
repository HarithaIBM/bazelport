# z/OS platform constraints

constraint_setting(name = "zos")

constraint_value(
    name = "is_zos",
    constraint_setting = ":zos",
    visibility = ["//visibility:public"],
)
