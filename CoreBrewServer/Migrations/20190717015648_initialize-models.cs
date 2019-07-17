using Microsoft.EntityFrameworkCore.Migrations;

namespace CoreBrewServer.Migrations
{
    public partial class initializemodels : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "BrewSchedules",
                columns: table => new
                {
                    Id = table.Column<int>(nullable: false)
                        .Annotation("Sqlite:Autoincrement", true),
                    Name = table.Column<string>(nullable: true),
                    MashTemperature = table.Column<float>(nullable: false),
                    MashDuration = table.Column<int>(nullable: false),
                    BoilDuration = table.Column<int>(nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_BrewSchedules", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "BrewDays",
                columns: table => new
                {
                    Id = table.Column<int>(nullable: false)
                        .Annotation("Sqlite:Autoincrement", true),
                    Name = table.Column<string>(nullable: true),
                    StartTime = table.Column<string>(nullable: true),
                    ReachedMashTemp = table.Column<string>(nullable: true),
                    CompletedMash = table.Column<string>(nullable: true),
                    ReachedBoil = table.Column<string>(nullable: true),
                    ComnpletedBoil = table.Column<string>(nullable: true),
                    BrewScheduleId = table.Column<int>(nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_BrewDays", x => x.Id);
                    table.ForeignKey(
                        name: "FK_BrewDays_BrewSchedules_BrewScheduleId",
                        column: x => x.BrewScheduleId,
                        principalTable: "BrewSchedules",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "MashTemperatures",
                columns: table => new
                {
                    Id = table.Column<int>(nullable: false)
                        .Annotation("Sqlite:Autoincrement", true),
                    Timestamp = table.Column<string>(nullable: true),
                    Temperature = table.Column<float>(nullable: false),
                    BrewDayId = table.Column<int>(nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_MashTemperatures", x => x.Id);
                    table.ForeignKey(
                        name: "FK_MashTemperatures_BrewDays_BrewDayId",
                        column: x => x.BrewDayId,
                        principalTable: "BrewDays",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_BrewDays_BrewScheduleId",
                table: "BrewDays",
                column: "BrewScheduleId");

            migrationBuilder.CreateIndex(
                name: "IX_MashTemperatures_BrewDayId",
                table: "MashTemperatures",
                column: "BrewDayId");
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "MashTemperatures");

            migrationBuilder.DropTable(
                name: "BrewDays");

            migrationBuilder.DropTable(
                name: "BrewSchedules");
        }
    }
}
